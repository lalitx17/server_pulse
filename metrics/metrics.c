#include "metrics.h"
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

char buffer[1024];

int count_processes() {
    int count = 0;
    DIR *procdir = opendir("/proc");
    struct dirent *entry;
    if (procdir) {
        while ((entry = readdir(procdir)) != NULL) {
            if (isdigit(entry->d_name[0])) {
                count++;
            }
        }
        closedir(procdir);
    }

    return count;
}

float kb_to_gb(float kb) { return kb / 1048576.0f; }

int get_cpu_metrics(cpu_metrics_t *cpu) {
    FILE *file = fopen("/proc/stat", "r");
    if (file == NULL) {
        perror("Failed to open /proc/stat");
    }

    unsigned long long prev_total = 0, prev_idle = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    char cpu_model[1024];

    fscanf(file, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice,
           &system, &idle, &iowait, &irq, &softirq, &steal);
    unsigned long long total =
        user + nice + system + idle + iowait + irq + softirq + steal;
    unsigned long long idle_time = idle + iowait;

    double cpu_usage = 100.0 * (total - prev_total - (idle_time - prev_idle)) /
                       (total - prev_total);
    fclose(file);

    cpu->cpu_usage_percent = cpu_usage;
    cpu->cpu_count = sysconf(_SC_NPROCESSORS_ONLN);

    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");

    if (cpuinfo) {
        while (fgets(buffer, sizeof(buffer), cpuinfo)) {
            if (!strncmp(buffer, "model name", 10)) {
                char *colon = strchr(buffer, ':');
                if (colon) {
                    strncpy(cpu_model, colon + 2, sizeof(cpu_model) - 1);
                    cpu_model[sizeof(cpu_model) - 1] = '\0';
                }
                break;
            }
        }
    }
    fclose(cpuinfo);
    strcpy(cpu->cpu_model, cpu_model);
    cpu->processes_count = count_processes();

    return 0;
}

int get_memory_metrics(memory_metrics_t *mem) {
    FILE *memfile = fopen("/proc/meminfo", "r");
    if (!memfile)
        return -1;

    float mem_total = 0, mem_free = 0, mem_cache = 0, swap_total = 0,
          swap_free = 0;

    while (fgets(buffer, sizeof(buffer), memfile)) {
        if (sscanf(buffer, "MemTotal: %f kB", &mem_total) == 1)
            continue;
        if (sscanf(buffer, "MemFree: %f kB", &mem_free) == 1)
            continue;
        if (sscanf(buffer, "Cached: %f kB", &mem_cache) == 1)
            continue;
        if (sscanf(buffer, "SwapTotal: %f kB", &swap_total) == 1)
            continue;
        if (sscanf(buffer, "SwapFree: %f kB", &swap_free) == 1)
            continue;
    }

    fclose(memfile);

    mem->total_memory = kb_to_gb(mem_total);
    mem->free_memory = kb_to_gb(mem_free);
    mem->cached_memory = kb_to_gb(mem_cache);
    mem->swap_total = kb_to_gb(swap_total);
    mem->swap_free = kb_to_gb(swap_free);
    mem->memory_pressure =
        (mem_total > 0) ? (1.0f - ((mem_free + mem_cache) / mem_total)) * 100.0f
                        : 0.0f;

    return 0;
}

int get_disk_metrics(disk_metrics_t *disk) {
    struct statvfs stat;
    if (statvfs("/", &stat) != 0) {
        return -1;
    }

    float total = (float)stat.f_blocks * stat.f_frsize / (1024 * 1024 * 1024);
    float free = (float)stat.f_bfree * stat.f_frsize / (1024 * 1024 * 1024);
    float used = total - free;
    float usage = (total > 0) ? (used / total) * 100.0f : 0.0f;

    disk->total_disk = total;
    disk->free_disk = free;
    disk->used_disk = used;
    disk->disk_usage = usage;
    return 0;
}
