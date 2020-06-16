#include "scullBuffer.h"

/* Parameters which can be set at load time */
int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_size = SCULL_SIZE;/* number of scull Buffer items */
int item_size = 512; /* the item size */

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_size, int, S_IRUGO);

MODULE_AUTHOR("Vishal/Dhass");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_buffer scullBufferDevice;/* instance of the scullBuffer structure */

/* file ops struct indicating which method is called for which io operation */
struct file_operations scullBuffer_fops = {
  .owner =    THIS_MODULE,
  .read =     scullBuffer_read,
  .write =    scullBuffer_write,
  .open =     scullBuffer_open,
  .release =  scullBuffer_release,
};

/*
 * Method used to allocate resources and set things up when the module
 * is being loaded.
 */
int scull_init_module(void){
  int result = 0;
  dev_t dev = 0;

  /* first check if someone has passed a major number */
  if (scull_major) {
    dev = MKDEV(scull_major, scull_minor);
    result = register_chrdev_region(dev, SCULL_NR_DEVS, "scullBuffer");
  } else {
    /* we need a dynamically allocated major number..*/
    result = alloc_chrdev_region(&dev, scull_minor, SCULL_NR_DEVS, "scullBuffer");
    scull_major = MAJOR(dev);
  }
  if (result < 0) {
    printk(KERN_WARNING "scullBuffer: can't get major %d\n", scull_major);
    return result;
  }

  /* alloc space for the buffer (scull_size bytes) */
  scullBufferDevice.bufferPtr = kmalloc( scull_size * item_size, GFP_KERNEL);
  scullBufferDevice.itemSizePtr = kmalloc( scull_size * sizeof(int),GFP_KERNEL);
  if(scullBufferDevice.bufferPtr == NULL||scullBufferDevice.itemSizePtr==NULL){
    scull_cleanup_module();
    return -ENOMEM;
  }

  /* Init the count vars */
  scullBufferDevice.readerCnt = 0;
  scullBufferDevice.writerCnt = 0;
  scullBufferDevice.size = 0;
  scullBufferDevice.nextItem = 0;
  scullBufferDevice.numItems = 0;

  /* Initialize the semaphore*/
  sema_init(&scullBufferDevice.semaphore_dev, 1);
  sema_init(&scullBufferDevice.semaphore_rw,1);
  sema_init(&scullBufferDevice.semaphore_full,0);
  sema_init(&scullBufferDevice.semaphore_empty,scull_size);
  /* Finally, set up the c dev. Now we can start accepting calls! */
  scull_setup_cdev(&scullBufferDevice);
  printk(KERN_DEBUG "scullBuffer: Init module done! The device is ready! buffer size= %d \n",scull_size*item_size);
  return result;
}

/*
 * Set up the char_dev structure for this device.
 * inputs: dev: Pointer to the device struct which holds the cdev
 */
static void scull_setup_cdev(struct scull_buffer *dev)
{
  int err, devno = MKDEV(scull_major, scull_minor);

  cdev_init(&dev->cdev, &scullBuffer_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &scullBuffer_fops;
  err = cdev_add (&dev->cdev, devno, 1);
  /* Fail gracefully if need be */
  if (err)
    printk(KERN_NOTICE "Error %d adding scullBuffer\n", err);
}

/*
 * Method used to cleanup the resources when the module is being unloaded
 * or when there was an initialization error
 */
void scull_cleanup_module(void)
{
  dev_t devno = MKDEV(scull_major, scull_minor);

  /* if buffer was allocated, get rid of it */
  if(scullBufferDevice.bufferPtr != NULL)
    kfree(scullBufferDevice.bufferPtr);

  if(scullBufferDevice.itemSizePtr != NULL)
    kfree(scullBufferDevice.itemSizePtr);

  /* Get rid of our char dev entries */
  cdev_del(&scullBufferDevice.cdev);

  /* cleanup_module is never called if registering failed */
  unregister_chrdev_region(devno, SCULL_NR_DEVS);

  printk(KERN_DEBUG "scullBuffer: cleanup module done! \n");
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);

/*
 * Implementation of the open system call
 */
int scullBuffer_open(struct inode *inode, struct file *filp)
{
  struct scull_buffer *dev;
  /* get and store the container scull_buffer */
  dev = container_of(inode->i_cdev, struct scull_buffer, cdev);
  filp->private_data = dev;

  if (down_interruptible(&dev->semaphore_rw))
    return -ERESTARTSYS;

  if (filp->f_mode & FMODE_READ){
    // increase # of consumer
    dev->readerCnt++;
  }
  if (filp->f_mode & FMODE_WRITE){
    // increase # of producer
    dev->writerCnt++;
  }
  up(&dev->semaphore_rw);
  return 0;
}

/*
 * Implementation of the close system call
 */
int scullBuffer_release(struct inode *inode, struct file *filp)
{
  struct scull_buffer *dev = (struct scull_buffer *)filp->private_data;
  if (down_interruptible(&dev->semaphore_rw) )
    return -ERESTARTSYS;

  if (filp->f_mode & FMODE_READ){
    dev->readerCnt--;//decrease # of consumer
  }
  if (filp->f_mode & FMODE_WRITE){
    dev->writerCnt--;//decrease # of producer
  }
  up(&dev->semaphore_rw);
  return 0;
}

/*
 * Implementation of the write system call
 */
ssize_t scullBuffer_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
  int countWritten = 0;
  int next_slot = 0;
  struct scull_buffer *dev = (struct scull_buffer *)filp->private_data;

  printk(KERN_DEBUG "write: %zd bytes to write\n",count);

  begin2:
  if (down_interruptible(&dev->semaphore_dev))
    return -ERESTARTSYS;

  if(count>item_size)
    count=item_size;

  // the buffer is full
  if(dev->numItems >= scull_size){
    printk(KERN_DEBUG "buffer is full, waiting...");
    if(down_interruptible(&dev->semaphore_rw)){
      up(&dev->semaphore_dev);
      return -ERESTARTSYS;
    }
    if(dev->readerCnt<=0){
      up(&dev->semaphore_rw);
      up(&dev->semaphore_dev);
      return 0;
    } else{
      up(&dev->semaphore_rw);
      up(&dev->semaphore_dev);
      //waiting for an item to read
      if(down_interruptible(&dev->semaphore_empty)){
	       return -ERESTARTSYS;
	    }
      up(&dev->semaphore_empty);
      goto begin2;
    }
  }

  next_slot = (dev->nextItem + dev->numItems)%scull_size;
  // fill the slot with the user data
  if(copy_from_user(dev->bufferPtr+next_slot*item_size,buf,count)){
    countWritten = -EFAULT;
    goto out2;
  }
  // update the number of items
  dev->numItems++;
  countWritten = count;
  dev->itemSizePtr[next_slot] = count;
  down(&dev->semaphore_empty);
  up(&dev->semaphore_full);
  printk(KERN_DEBUG "%zd bytes written, numItems = %d, slot = %d\n",count, dev->numItems, next_slot);

  out2:
  up(&dev->semaphore_dev);
  return countWritten;
}

/*
 * Implementation of the read system call
 */
ssize_t scullBuffer_read(
			 struct file *filp,
			 char __user *buf,
			 size_t count,
			 loff_t *f_pos)
{
  struct scull_buffer *dev = (struct scull_buffer *)filp->private_data;
  ssize_t countRead = 0;
  int nextItemSize=0;

  /* get exclusive access */
 begin1:
 if (down_interruptible(&dev->semaphore_dev))
    return -ERESTARTSYS;

  if(count>item_size)
    count = item_size;

  if(dev->numItems<=0){
    if(down_interruptible(&dev->semaphore_rw)){
      up(&dev->semaphore_dev);
      return -ERESTARTSYS;
    }
    if(dev->writerCnt>0){
      // waiting for an item to write
      printk(KERN_DEBUG "read: waiting for item");
      up(&dev->semaphore_rw);
      up(&dev->semaphore_dev);
      if(down_interruptible(&dev->semaphore_full)){
	       return -ERESTARTSYS;
      }
      up(&dev->semaphore_full);
      // go to the beginning
      goto begin1;
    }else{
      printk(KERN_DEBUG "read: there is no writer, so return 0");
      up(&dev->semaphore_rw);
      up(&dev->semaphore_dev);
      return 0;
    }
  }else{
    printk(KERN_DEBUG "\nscullBuffer: read called count= %zd\n", count);
    printk(KERN_DEBUG "before read, numItems:%d, nextItem:%d, position:%lld\n",dev->numItems,dev->nextItem,*f_pos);
    nextItemSize  = *(dev->itemSizePtr+dev->nextItem);
    if (nextItemSize < count) {
      count = nextItemSize;
    }
    if(copy_to_user(buf,dev->bufferPtr+(loff_t)(dev->nextItem*item_size), count)){
      countRead = -EFAULT;
      goto out1;
    }
    // update the item pointer to the next possible item
    dev->nextItem = (dev->nextItem+1) % scull_size;
    // decrease the number of items
    dev->numItems --;
    // decrease the full counter
    down_interruptible(&dev->semaphore_full);
    // increase the empty counter
    up(&dev->semaphore_empty);
    countRead = nextItemSize;
    printk(KERN_DEBUG "after read,numItems:%d, nextItem:%d\n\n",dev->numItems,dev->nextItem);
  }

  /* now we're done release the semaphore */
 out1:
  up(&dev->semaphore_dev);
  return countRead;
}
