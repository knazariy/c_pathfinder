#pragma once

#include "libmx.h"

#define ERR_ARG_COUNT -1
#define ERR_FILE_NOT_EXISTS -2
#define ERR_FILE_IS_EMPTY -3
#define ERR_NUM_OF_ISLANDS -4
#define ERR_DUPLICATE_BRIDGES -5
#define ERR_LEN_SUM_TOO_BIG -6

#define MAX_INT 2147483647

// TODO price history, showing equal paths
typedef struct s_graph {
    int island_count;
    long len_sum;
    int last_filled_index;
    char **islands;
    char **literal_paths;
    char **literal_distances;
    int *init_weights;
    bool *has_proxy;
    int *weights;
} t_graph;

bool handle_errors(int argc, char **argv, t_graph *graph);
void report_error(int errno, char *filename);
void find_paths(t_graph *graph);
void output_graph_matrix(t_graph *graph);
void output_graph_routes(t_graph *graph);
void set_initial_path(t_graph *graph, int isl1_index, int isl2_index);
void set_initial_distance(t_graph *graph, int isl1_index, int isl2_index);
char *build_literal_path(char *from_proxy, char *proxy_to);
char *build_literal_distance(char *from_proxy_raw, char *proxy_to_raw, int weight);
