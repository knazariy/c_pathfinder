#include "pathfinder.h"

bool check_args(int argc) {
    if (argc != 2) {
        report_error(ERR_ARG_COUNT, NULL);
        return false;
    }
    return true;
}

bool check_file_exists(char *filename) {
    if (!mx_file_exists(filename)) {
        report_error(ERR_FILE_NOT_EXISTS, filename);
        return false;
    }
    return true;
}

bool check_file_is_empty(char *filename) {
    if (mx_file_is_empty(filename)) {
        report_error(ERR_FILE_IS_EMPTY, filename);
        return false;
    }
    return true;
}

bool first_line_correct(char *first_line) {
    long first_line_as_long;

    // check for non-digits for first line
    for (int i = 0; i < mx_strlen(first_line); i++) {
        if (!mx_isdigit(first_line[i])) {
            return false;
        }
    }
    first_line_as_long = mx_atol(first_line);
    // check for negative
    if (first_line_as_long < 1) {
        return false;
    }
    return true;
}

// TODO check line (duplicates)
bool check_bridge_syntax(char *line) {
    bool comma_met = false;
    bool hyphen_met = false;
    bool digit_met = false;

    for (int i = 0; i < mx_strlen(line); i++) {
        if (!mx_isalpha(line[i])) {
            if (line[i] != ',' && line[i] != '-') {
                if (hyphen_met && comma_met && mx_isdigit(line[i])) {
                    digit_met = true;
                } else {
                    return false;
                }
            }
            if (line[i] == ',') {
                if (comma_met) return false;
                else comma_met = true;
            }
            if (line[i] == '-') {
                if (hyphen_met) return false;
                else hyphen_met = true;
            }
        } else if (digit_met) return false;
    }
    return true;
}

// result[0] - island1, result[1] - island2, result[2] - cost
char **parse_islands(char *line) {
    char **hyphen_separated = mx_strsplit(line, '-');
    char *island1 = hyphen_separated[0];
    char *island2;
    char **temp = mx_strsplit(hyphen_separated[1], ',');
    char** result = malloc(sizeof(char *) * 4);

    island2 = temp[0];
    result[0] = mx_strdup(island1);
    result[1] = mx_strdup(island2);
    result[2] = mx_strdup(temp[1]);
    result[3] = NULL;

    mx_del_strarr(&temp);
    mx_del_strarr(&hyphen_separated);
    return result;
}

bool check_cost_already_filled(int island1_index, int island2_index, t_graph *graph) {
    int *weights = graph->weights;

    if (weights[island1_index * graph->island_count + island2_index] != -1 ||
        weights[island2_index * graph->island_count + island1_index] != -1) {
        return true;
    }
    return false;
}

int parse_bridge_syntax(char *line, t_graph *graph) {
    char **islands;
    char *island1;
    char *island2;
    int island1_index;
    int island2_index;
    long cost;

    if (mx_strlen(line) < 5) return -1;
    if (!check_bridge_syntax(line)) return -1;
    islands = parse_islands(line);
    island1 = islands[0];
    island2 = islands[1];
    if (mx_strcmp(island1, island2) == 0) return -1;
    cost = mx_atol(islands[2]);
    graph->len_sum += cost;
    // TODO add island1, island2 to graph
    //  adding island names if they're not there
    if (!mx_contains_str(island1, (const char **) graph->islands)) {
        if (graph->last_filled_index >= graph->island_count) return ERR_NUM_OF_ISLANDS;
        graph->islands[graph->last_filled_index++] = mx_strdup(island1);
    }
    if (!mx_contains_str(island2, (const char **) graph->islands)) {
        if (graph->last_filled_index >= graph->island_count) return ERR_NUM_OF_ISLANDS;
        graph->islands[graph->last_filled_index++] = mx_strdup(island2);
    }
    island1_index = mx_get_str_index((const char *) island1,
                                     (const char **) graph->islands);
    island2_index = mx_get_str_index((const char *) island2,
                                     (const char **) graph->islands);
    //set cost between islands
    if (check_cost_already_filled(island1_index, island2_index, graph))
        return ERR_DUPLICATE_BRIDGES;
    graph->weights[island1_index * graph->island_count + island2_index] =
            (int) cost;
    graph->weights[island2_index * graph->island_count + island1_index] =
            (int) cost;
    mx_del_strarr(&islands);
    return 0;
}

void fill_weights(int *weights, int arr_size) {
    for (int i = 0; i < arr_size; i++) {
        for (int j = 0; j < arr_size; j++) {
            weights[arr_size * i + j] = -1;
        }
    }
}

int check_lines(char *filename, t_graph *graph) {
    int fd;
    char *current_line = mx_strnew(100);
    int line_count = 0;
    int parse_result = 0;

    fd = open(filename, O_RDONLY);
    mx_read_line(&current_line, 1, '\n', fd);
    if (!first_line_correct(current_line)) {
        mx_strdel(&current_line);
        return 1;
    }
    graph->island_count = mx_atoi(current_line);
    graph->islands = (char **)malloc(sizeof(char *) * (graph->island_count + 1));
    graph->islands[graph->island_count] = NULL;
    graph->weights = (int *)malloc(sizeof(int) * (graph->island_count * graph->island_count));
    fill_weights(graph->weights, graph->island_count);
    while (mx_read_line(&current_line ,1, '\n', fd) != -1) {
        parse_result = parse_bridge_syntax(current_line, graph);
        if (parse_result == -1) {
            return line_count + 2;
        } else if (parse_result != 0) {
            return parse_result;
        }
        if (graph->len_sum > MAX_INT) {
            return ERR_LEN_SUM_TOO_BIG;
        }
        line_count++;
    }
//    for (int i = 0; i < graph->island_count; i++) {
//        if (mx_read_line(&current_line, 1, '\n', fd) < 0) {
//            return ERR_NUM_OF_ISLANDS;
//        }
//        if (!parse_bridge_syntax(current_line, graph)) {
//            return i + 2;
//        }
//        if (graph->len_sum > MAX_INT) {
//            return ERR_LEN_SUM_TOO_BIG;
//        }
//        // TODO put islands into graph
//        // TODO check for duplicates
//    }
//    // if hasn't reached EOF
//    if (mx_read_line(&current_line, 1, '\n', fd) != -1) {
//        return ERR_NUM_OF_ISLANDS;
//    }
    mx_strdel(&current_line);
    return 0;
}

// checks arg count, whether file exists or empty
// checks first line (and sets island_count)
bool handle_errors(int argc, char **argv, t_graph *graph) {
    char *filename;

    (void) graph;
    if (!check_args(argc)) return false;
    filename = argv[1];
    if (!check_file_exists(filename)) return false;
    if (!check_file_is_empty(filename)) return false;
    int incorrect_line = check_lines(filename, graph);
    if (incorrect_line != 0) {
        report_error(incorrect_line, filename);
        return false;
    }

    return true;
}
