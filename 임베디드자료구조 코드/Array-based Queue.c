#define MAX 10
int queue[MAX];
int front, rear;

void init_queue()
{
	front = 0;
	rear = 0;
}
void clear_queue()
{
	front = rear;
}
void print_queue()
{
	int k;
	for (k = front; k != rear; k = ++k % MAX)
		printf(" % -6d", queue[k]);
}
int put(int k)
{
	if ((rear + 1) % MAX == front)
	{
		printf("Queue overflow !!!\n");
		return -1;
	}
	rear = ++rear % MAX;
	return k;
}
int get()
{
	int j;
	if (front == rear)
	{
		printf("Queue underflow !!!\n");
		return -1;
	}
	j = queue[front];
	front = ++front % MAX;
	return j;
}

void main()
{
	int k;
	init_queue();
}