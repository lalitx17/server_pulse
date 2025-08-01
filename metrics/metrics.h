#ifndef METRICS_H
#define METRICS_H

typedef struct cpu_metrics {
    float cpu_usage_percent;
    int cpu_count;
    char cpu_model[1024];
    int processes_count;
} cpu_metrics_t;

typedef struct memory_metrics {
    float total_memory;
    float free_memory;
    float cached_memory;
    float swap_total;
    float swap_free;
    float memory_pressure;
} memory_metrics_t;

typedef struct disk_metrics {
    float total_disk;
    float free_disk;
    float used_disk;
    float disk_usage;
} disk_metrics_t;

int get_cpu_metrics(cpu_metrics_t *cpu);
int get_memory_metrics(memory_metrics_t *mem);
int get_disk_metrics(disk_metrics_t *disk);
char *get_cpu_metrics_json();
char *get_memory_metrics_json();
char *get_disk_metrics_json();

#endif