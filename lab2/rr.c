#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 remaining_total_time;
  u32 run_remaining_time;
  u32 response_time;
  u32 waiting_time;
  bool not_run_yet;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  //Helper Function 1
  int calculate_time_slice(int remaining_time, int quanta){
    if(remaining_time>quanta){
      return quanta;
    }
    else{
      return remaining_time;
    }
  }
  u32 current_time = 0;
  u32 num_finished_processes = 0;
  struct process *current_process = NULL;  // Ensure current_process is initialized to NULL

  while (num_finished_processes < size) {
    // Check which processes have arrived and append to the linked list
    for (int i = 0; i < size; i++) {
        if (current_time == data[i].arrival_time) {
            if (data[i].burst_time <= 0) {
                // Process has zero burst time, consider it finished
                num_finished_processes++;
                continue;
            }
            struct process *new_node = malloc(sizeof(struct process));
            if (new_node == NULL) {
                perror("Failed to allocate memory for new_node");
                exit(EXIT_FAILURE);
            }
            // Main parameters
            new_node->pid = data[i].pid;
            new_node->arrival_time = data[i].arrival_time;
            new_node->burst_time = data[i].burst_time;
            // Extra parameters
            new_node->remaining_total_time = data[i].burst_time;
            new_node->waiting_time = 0;
            new_node->not_run_yet = true;
            // Define remaining time
            new_node->run_remaining_time = calculate_time_slice(new_node->remaining_total_time, quantum_length);
            // Insert new node at the head of the list
            TAILQ_INSERT_HEAD(&list, new_node, pointers);
        }
    }


  // Check if the list is not empty before proceeding with Round Robin logic
  if (!TAILQ_EMPTY(&list)) {
    // Case 1: The currently running process has time left in its time slice => keep running `current_process`
    if (current_process != NULL && current_process->run_remaining_time > 0) {
      current_process->remaining_total_time -= 1;
      current_process->run_remaining_time -= 1;
    }
    // Case 2: Time slice expired
    else if(current_process != NULL && current_process->run_remaining_time <= 0){
      struct process *temp = TAILQ_LAST(&list, process_list);
    
      // Case 1: Process has completed its total runtime, remove from the queue
      if (temp->remaining_total_time <= 0) {
        TAILQ_REMOVE(&list, temp, pointers);
        num_finished_processes++;
        free(temp); 
        current_process = NULL;
      }
      // Case 2: Process has time left, reinsert it at the head for Round Robin
      else {
        TAILQ_REMOVE(&list, temp, pointers);
        TAILQ_INSERT_HEAD(&list, temp, pointers);
      }
      //Schedule the next process if queue not empty
      if (!TAILQ_EMPTY(&list)){
        temp = TAILQ_LAST(&list, process_list);
        // Update `current_process` to point to the reinserted process
        current_process = temp;
        // Set the new time slice for the process
        temp->run_remaining_time = calculate_time_slice(temp->remaining_total_time, quantum_length);
        // If the process has not yet run, set its response time
        if (temp->not_run_yet == true) {
            temp->response_time = current_time - (temp->arrival_time);
            total_response_time += temp->response_time;
            temp->not_run_yet = false;
        }
        // Decrement `current_process` times immediately after setting it
        current_process->run_remaining_time -= 1;
        current_process->remaining_total_time -= 1;
      }
    }
    // Case 3: Current Process is empty. Schedule the next process on the list
    else{
      struct process *temp = TAILQ_LAST(&list, process_list);
      //Schedule the next process if queue not empty
      if (!TAILQ_EMPTY(&list)){
        temp = TAILQ_LAST(&list, process_list);
        // Update `current_process` to point to the reinserted process
        current_process = temp;
        // Set the new time slice for the process
        temp->run_remaining_time = calculate_time_slice(temp->remaining_total_time, quantum_length);
        // If the process has not yet run, set its response time
        if (temp->not_run_yet == true) {
            temp->response_time = current_time - (temp->arrival_time);
            total_response_time += temp->response_time;
            temp->not_run_yet = false;
        }
        // Decrement `current_process` times immediately after setting it
        current_process->run_remaining_time -= 1;
        current_process->remaining_total_time -= 1;
      }


    }
  }
  
  // Increment the wait time for all processes except `current_process`
  struct process *temp;
  TAILQ_FOREACH(temp, &list, pointers) {
      if (temp != current_process) {
          temp->waiting_time += 1;
          total_waiting_time += 1;
      }
  }
  // Increment time
  current_time++;
  }

  //Remove all elements and free all memory
  while(!TAILQ_EMPTY(&list)){
    struct process *to_remove = TAILQ_FIRST(&list);
    TAILQ_REMOVE(&list,to_remove, pointers);
    free(to_remove);
  }

  /* End of "Your code here" */
  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
