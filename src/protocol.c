#include "protocol.h"

void get_current_timestamp(char *buffer)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 30, "%H:%M:%S", t);
}

void serialize_packet(ChatPacket *pkt, char *buffer)
{
    memcpy(buffer, pkt, sizeof(ChatPacket));
}

void deserialize_packet(char *buffer, ChatPacket *pkt)
{
    memcpy(pkt, buffer, sizeof(ChatPacket));
}

int convert_be_to_ad(int be_year)
{
    return be_year - 543;
}

char student_name[] = "Somchai";
char student_surname[] = "Jaidee";
char institute[] = "Thammasat school of Engineering";

void *thread_print_name(void *arg)
{
    printf("[Thread %lu] Name: %s\n", (unsigned long)pthread_self(), student_name);
    return NULL;
}

void *thread_print_surname(void *arg)
{
    printf("[Thread %lu] Surname: %s\n", (unsigned long)pthread_self(), student_surname);
    return NULL;
}

void *thread_print_institute(void *arg)
{
    printf("[Thread %lu] Institute: %s\n", (unsigned long)pthread_self(), institute);
    return NULL;
}

void *thread_count_chars(void *arg)
{
    int count = strlen(institute);
    printf("[Thread %lu] Character count of Institute: %d\n", (unsigned long)pthread_self(), count);
    return NULL;
}

void run_hw2_part1()
{
    printf("\n--- Running HW2 Part 1 ---\n");
    pthread_t t1, t2, t3, t4;

    pthread_create(&t1, NULL, thread_print_name, NULL);
    pthread_create(&t2, NULL, thread_print_surname, NULL);
    pthread_create(&t3, NULL, thread_print_institute, NULL);
    pthread_create(&t4, NULL, thread_count_chars, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    printf("-------------------------\n\n");
}

typedef struct
{
    int thread_id;
    long max_value;
} CountArg;

void *thread_count_competition(void *arg)
{
    CountArg *c_arg = (CountArg *)arg;
    for (long i = 0; i <= c_arg->max_value; i++)
    {
        // Counting...
    }
    printf("Thread %d finished counting to %ld!\n", c_arg->thread_id, c_arg->max_value);
    return NULL;
}

void run_hw2_part2(long max_value)
{
    printf("\n--- Running HW2 Part 2 (16 Threads) ---\n");
    pthread_t threads[16];
    CountArg args[16];

    for (int i = 0; i < 16; i++)
    {
        args[i].thread_id = i + 1;
        args[i].max_value = max_value;
        pthread_create(&threads[i], NULL, thread_count_competition, &args[i]);
    }

    for (int i = 0; i < 16; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("--------------------------------------\n\n");
}

int extract_and_convert_year(const char *input_msg)
{
    int len = strlen(input_msg);

    if (len >= 8)
    {
        char year_str[5];

        strncpy(year_str, input_msg + (len - 4), 4);
        year_str[4] = '\0';

        int be_year = atoi(year_str);
        if (be_year > 2000)
        {
            return be_year - 543;
        }
    }
    return -1;
}