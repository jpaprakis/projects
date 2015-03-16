int mymalloc_init() {

  sem_init(&sem, 0, 1);
  sem_wait(&sem);
  void *new_mem_ptr;
  if ((new_mem_ptr = sbrk(SBRK_MULTI)) == (void*) -1)
  {
    sem_post(&sem);
    return 1;
  }
  
  //pthread_mutex_lock(&free_lock);
//Pointing to the address at new_mem_ptr to store data
  free_head = new_mem_ptr;
  free_head->size = SBRK_MULTI;
  free_head->next = NULL;

  allocated_head = NULL;
  sem_post(&sem);
  //pthread_mutex_unlock(&free_lock);
  //pthread_mutex_init(&free_lock,  NULL);  
  return 0; 
}
