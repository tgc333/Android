#include <unistd.h>
#include <pthread.h> 
#include <semaphore.h>
#include "stems.h"
#include "request.h"

typedef int element;

typedef struct tagNode
{
    element fd;
    long arrivalTime;
}Node;

typedef struct tagCircularQueue
{
    int capacity;
    int front;
    int rear;
    Node* Nodes;
} CircularQueue;

int count=0;

CircularQueue* queue;
sem_t S, Full, Empty;

void CreateQueue(CircularQueue** queue, int capacity)
{
    (*queue) = (CircularQueue*)malloc(sizeof(CircularQueue));
    (*queue)->Nodes = (Node*)malloc(sizeof(Node)* (capacity + 1));
    (*queue)->capacity = capacity;
    (*queue)->front = 0;
    (*queue)->rear = 0;
}

void DestroyQueue(CircularQueue* queue)
{
    free(queue->Nodes);
    free(queue);
}

void Enqueue(CircularQueue* queue, element fd, long arrivalTime)
{
    int position = 0;

    if (queue->rear == queue->capacity)
    {
        position = queue->rear;
        queue->rear = 0;
    }
    else
        position = queue->rear++;

    queue->Nodes[position].fd = fd;
    queue->Nodes[position].arrivalTime = arrivalTime;
}

Node Dequeue(CircularQueue* queue)
{
    int position = queue->front;

    if (queue->front == queue->capacity)
        queue->front = 0;
    else
        queue->front++;

    return queue->Nodes[position];
}

int IsEmpty(CircularQueue* queue)
{
    return (queue->front == queue->rear);
}

int IsFull(CircularQueue* queue)
{
    if (queue->front < queue->rear)
        return (queue->rear - queue->front) == queue->capacity;
    else
        return (queue->rear + 1) == queue->front;
}


void getargs_ws(int *port, int *thread_size, int *q_size)
{
  FILE *fp;

  if ((fp = fopen("config-ws.txt", "r")) == NULL)
    unix_error("config-ws.txt file does not open.");

  fscanf(fp, "%d", port);
  fscanf(fp, "%d", thread_size);
  fscanf(fp, "%d", q_size);
  fclose(fp);
}

void consumer(int connfd, long arrivalTime, int id)
{
  requestHandle(connfd, arrivalTime, id);
  Close(connfd);
}

void *exec_thread(void *data) {
  Node node;
  int id = *(int*)data;
  char fifo_name[MAXBUF];
  
  sprintf(fifo_name,"./fifo%d", id);

  while(1) {
    sem_wait(&Empty);
    sem_wait(&S);
    node = Dequeue(queue);
    sem_post(&S);
    sem_post(&Full);

    consumer(node.fd, node.arrivalTime, id);
    char *method = getenv("REQUEST_METHOD");
    if(strcasecmp(method, "POST")==0){
      pid_t pid;
      pid = Fork();
      if(pid == 0){
          Setenv("FIFO_NAME", fifo_name, 1);
          Execve("./alarmClient", (NULL) , environ);
      }
    }
  }
}

int main(void)
{
  int listenfd, connfd, port, clientlen, q_size, thread_size, i;
  struct sockaddr_in clientaddr;
  pthread_t *thread_id;
  int *arr;
  initWatch();
  getargs_ws(&port, &thread_size, &q_size);
  listenfd = Open_listenfd(port);

  CreateQueue(&queue, q_size);

  arr = (int *)malloc(sizeof(int)*thread_size);
  thread_id = (pthread_t*)malloc(sizeof(pthread_t)*thread_size);
  sem_init(&S, 0, 1);
  sem_init(&Full, 0, q_size);
  sem_init(&Empty, 0, 0);

  for (i = 0; i<thread_size; i++) {
      arr[i]=i;
      pthread_create(&thread_id[i], NULL, &exec_thread, (void *)&arr[i]);
      pthread_detach(thread_id[i]);
  }

  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    
    sem_wait(&Full);
    sem_wait(&S);
    Enqueue(queue, connfd, getWatch());
    sem_post(&S);
    sem_post(&Empty);    
  }
  return(0);
}
