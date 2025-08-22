#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_TODOS 1000
#define MAX_DESC 256
#define MAX_DATE 11
#define TODO_FILE "todos.txt"
#define MAX_LINE 512

// Colors for beautiful terminal output
#define RESET   "\033[0m"
#define BRIGHT  "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BG_RED  "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"

typedef struct {
    char id[20];
    char description[MAX_DESC];
    int completed;
    char priority[10];
    char dueDate[MAX_DATE];
    char createdAt[20];
    char completedAt[20];
} Todo;

typedef struct {
    Todo todos[MAX_TODOS];
    int count;
} TodoList;

TodoList todoList;

// Function prototypes
void initTodoList();
void loadTodos();
void saveTodos();
void addTodo(const char* description, const char* priority, const char* dueDate);
void listTodos(const char* filter);
void completeTodo(const char* id);
void deleteTodo(const char* id);
void searchTodos(const char* query);
void showStats();
void showHelp();
void clearScreen();
void printHeader();
void printPrompt();
char* getPriorityColor(const char* priority);
int isValidDate(const char* date);
void generateId(char* id);
void getCurrentTime(char* timeStr);
void trim(char* str);

int main() {
    char input[512];
    char command[50];
    char args[3][256];
    int i, argCount;

    initTodoList();
    loadTodos();

    clearScreen();
    printHeader();

    while (1) {
        printPrompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) continue;

        // Parse command and arguments
        argCount = 0;
        char* token = strtok(input, " ");
        if (token) {
            strcpy(command, token);
            token = strtok(NULL, " ");
            while (token && argCount < 3) {
                strcpy(args[argCount], token);
                argCount++;
                token = strtok(NULL, " ");
            }
        }

        // Convert command to lowercase
        for (i = 0; command[i]; i++) {
            command[i] = tolower(command[i]);
        }

        if (strcmp(command, "add") == 0) {
            if (argCount == 0) {
                printf(RED "Error:" RESET " Please provide a description for the todo.\n");
                continue;
            }
            char* priority = (argCount > 1) ? args[1] : "medium";
            char* dueDate = (argCount > 2) ? args[2] : "";
            addTodo(args[0], priority, dueDate);
        }
        else if (strcmp(command, "list") == 0) {
            char* filter = (argCount > 0) ? args[0] : "all";
            listTodos(filter);
        }
        else if (strcmp(command, "complete") == 0) {
            if (argCount == 0) {
                printf(RED "Error:" RESET " Please provide a todo ID.\n");
                continue;
            }
            completeTodo(args[0]);
        }
        else if (strcmp(command, "delete") == 0) {
            if (argCount == 0) {
                printf(RED "Error:" RESET " Please provide a todo ID.\n");
                continue;
            }
            deleteTodo(args[0]);
        }
        else if (strcmp(command, "search") == 0) {
            if (argCount == 0) {
                printf(RED "Error:" RESET " Please provide a search query.\n");
                continue;
            }
            searchTodos(args[0]);
        }
        else if (strcmp(command, "stats") == 0) {
            showStats();
        }
        else if (strcmp(command, "help") == 0) {
            showHelp();
        }
        else if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            printf("\n" GREEN "👋 Goodbye!" RESET "\n\n");
            break;
        }
        else if (strcmp(command, "clear") == 0) {
            clearScreen();
            printHeader();
        }
        else {
            printf(RED "Unknown command:" RESET " %s\n", command);
            printf("Type 'help' for available commands.\n");
        }
    }

    return 0;
}

void initTodoList() {
    todoList.count = 0;
    memset(todoList.todos, 0, sizeof(todoList.todos));
}

void loadTodos() {
    FILE* file = fopen(TODO_FILE, "r");
    if (!file) return;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file) && todoList.count < MAX_TODOS) {
        Todo* todo = &todoList.todos[todoList.count];

        // Parse line: id|description|completed|priority|dueDate|createdAt|completedAt
        char* token = strtok(line, "|");
        if (token) {
            strcpy(todo->id, token);
            trim(todo->id);

            token = strtok(NULL, "|");
            if (token) {
                strcpy(todo->description, token);
                trim(todo->description);

                token = strtok(NULL, "|");
                if (token) {
                    todo->completed = atoi(token);

                    token = strtok(NULL, "|");
                    if (token) {
                        strcpy(todo->priority, token);
                        trim(todo->priority);

                        token = strtok(NULL, "|");
                        if (token) {
                            strcpy(todo->dueDate, token);
                            trim(todo->dueDate);

                            token = strtok(NULL, "|");
                            if (token) {
                                strcpy(todo->createdAt, token);
                                trim(todo->createdAt);

                                token = strtok(NULL, "|");
                                if (token) {
                                    strcpy(todo->completedAt, token);
                                    trim(todo->completedAt);
                                }
                            }
                        }
                    }
                }
            }
        }

        todoList.count++;
    }

    fclose(file);
}

void saveTodos() {
    FILE* file = fopen(TODO_FILE, "w");
    if (!file) {
        printf(RED "Error saving todos!" RESET "\n");
        return;
    }

    for (int i = 0; i < todoList.count; i++) {
        Todo* todo = &todoList.todos[i];
        fprintf(file, "%s|%s|%d|%s|%s|%s|%s\n",
                todo->id, todo->description, todo->completed,
                todo->priority, todo->dueDate, todo->createdAt, todo->completedAt);
    }

    fclose(file);
}

void addTodo(const char* description, const char* priority, const char* dueDate) {
    if (todoList.count >= MAX_TODOS) {
        printf(RED "Error:" RESET " Maximum number of todos reached.\n");
        return;
    }

    Todo* todo = &todoList.todos[todoList.count];

    generateId(todo->id);
    strcpy(todo->description, description);
    todo->completed = 0;
    strcpy(todo->priority, priority);
    strcpy(todo->dueDate, dueDate);
    getCurrentTime(todo->createdAt);
    strcpy(todo->completedAt, "");

    todoList.count++;
    saveTodos();

    printf(GREEN "✓" RESET " Todo added: " CYAN "%s" RESET "\n", description);
}

void listTodos(const char* filter) {
    if (todoList.count == 0) {
        printf(YELLOW "No todos found." RESET "\n");
        return;
    }

    int filteredCount = 0;
    Todo filteredTodos[MAX_TODOS];

    // Apply filter
    for (int i = 0; i < todoList.count; i++) {
        Todo* todo = &todoList.todos[i];
        int include = 0;

        if (strcmp(filter, "all") == 0) {
            include = 1;
        } else if (strcmp(filter, "pending") == 0 && !todo->completed) {
            include = 1;
        } else if (strcmp(filter, "completed") == 0 && todo->completed) {
            include = 1;
        } else if (strcmp(filter, "high") == 0 && strcmp(todo->priority, "high") == 0) {
            include = 1;
        } else if (strcmp(filter, "medium") == 0 && strcmp(todo->priority, "medium") == 0) {
            include = 1;
        } else if (strcmp(filter, "low") == 0 && strcmp(todo->priority, "low") == 0) {
            include = 1;
        }

        if (include) {
            filteredTodos[filteredCount] = *todo;
            filteredCount++;
        }
    }

    if (filteredCount == 0) {
        printf(YELLOW "No todos found for filter: %s" RESET "\n", filter);
        return;
    }

    printf("\n" BRIGHT BLUE "📋 Todo List" RESET " (%d items)\n\n", filteredCount);

    for (int i = 0; i < filteredCount; i++) {
        Todo* todo = &filteredTodos[i];
        char* status = todo->completed ? GREEN "✓" RESET : YELLOW "○" RESET;
        char* priorityColor = getPriorityColor(todo->priority);
        char* priority = todo->priority;

        printf("%s %d. %s %s%s%s", status, i + 1, todo->description,
               priorityColor, priority, RESET);

        if (strlen(todo->dueDate) > 0) {
            printf(" " MAGENTA "📅 %s" RESET, todo->dueDate);
        }

        if (todo->completed && strlen(todo->completedAt) > 0) {
            printf(" " GREEN "✅ Completed" RESET);
        }

        printf("\n");
    }
    printf("\n");
}

void completeTodo(const char* id) {
    for (int i = 0; i < todoList.count; i++) {
        Todo* todo = &todoList.todos[i];
        if (strcmp(todo->id, id) == 0) {
            if (todo->completed) {
                todo->completed = 0;
                strcpy(todo->completedAt, "");
                printf(YELLOW "○" RESET " Todo unmarked: " CYAN "%s" RESET "\n", todo->description);
            } else {
                todo->completed = 1;
                getCurrentTime(todo->completedAt);
                printf(GREEN "✓" RESET " Todo completed: " CYAN "%s" RESET "\n", todo->description);
            }
            saveTodos();
            return;
        }
    }
    printf(RED "✗" RESET " Todo not found.\n");
}

void deleteTodo(const char* id) {
    for (int i = 0; i < todoList.count; i++) {
        Todo* todo = &todoList.todos[i];
        if (strcmp(todo->id, id) == 0) {
            printf(RED "🗑️" RESET " Todo deleted: " CYAN "%s" RESET "\n", todo->description);

            // Shift remaining todos
            for (int j = i; j < todoList.count - 1; j++) {
                todoList.todos[j] = todoList.todos[j + 1];
            }

            todoList.count--;
            saveTodos();
            return;
        }
    }
    printf(RED "✗" RESET " Todo not found.\n");
}

void searchTodos(const char* query) {
    int results[MAX_TODOS];
    int resultCount = 0;

    // Find matching todos
    for (int i = 0; i < todoList.count; i++) {
        Todo* todo = &todoList.todos[i];
        if (strstr(todo->description, query) != NULL) {
            results[resultCount] = i;
            resultCount++;
        }
    }

    if (resultCount == 0) {
        printf(YELLOW "No todos found matching: %s" RESET "\n", query);
        return;
    }

    printf("\n" BRIGHT BLUE "🔍 Search Results" RESET " for \"%s\" (%d items)\n\n", query, resultCount);

    for (int i = 0; i < resultCount; i++) {
        Todo* todo = &todoList.todos[results[i]];
        char* status = todo->completed ? GREEN "✓" RESET : YELLOW "○" RESET;
        char* priorityColor = getPriorityColor(todo->priority);

        printf("%s %d. %s %s%s%s\n", status, i + 1, todo->description,
               priorityColor, todo->priority, RESET);
    }
    printf("\n");
}

void showStats() {
    int total = todoList.count;
    int completed = 0;
    int highPriority = 0;

    for (int i = 0; i < todoList.count; i++) {
        Todo* todo = &todoList.todos[i];
        if (todo->completed) completed++;
        if (strcmp(todo->priority, "high") == 0 && !todo->completed) highPriority++;
    }

    int pending = total - completed;

    printf("\n" BRIGHT BLUE "📊 Todo Statistics" RESET "\n\n");
    printf("Total: " CYAN "%d" RESET "\n", total);
    printf("Completed: " GREEN "%d" RESET "\n", completed);
    printf("Pending: " YELLOW "%d" RESET "\n", pending);
    printf("High Priority Pending: " RED "%d" RESET "\n", highPriority);

    if (total > 0) {
        float completionRate = (float)completed / total * 100;
        printf("Completion Rate: " CYAN "%.1f%%" RESET "\n", completionRate);
    }
    printf("\n");
}

void showHelp() {
    printf("\n" BRIGHT BLUE "🚀 Todo CLI - Help" RESET "\n\n");
    printf(CYAN "Commands:" RESET "\n");
    printf("  " GREEN "add" RESET " <description> [priority] [due-date]  - Add a new todo\n");
    printf("  " GREEN "list" RESET " [filter]                          - List todos (all/pending/completed/high/medium/low)\n");
    printf("  " GREEN "complete" RESET " <id>                          - Toggle todo completion\n");
    printf("  " GREEN "delete" RESET " <id>                           - Delete a todo\n");
    printf("  " GREEN "search" RESET " <query>                        - Search todos\n");
    printf("  " GREEN "stats" RESET "                                 - Show statistics\n");
    printf("  " GREEN "help" RESET "                                  - Show this help\n");
    printf("  " GREEN "clear" RESET "                                 - Clear screen\n");
    printf("  " GREEN "quit" RESET "                                  - Exit the app\n\n");

    printf(CYAN "Examples:" RESET "\n");
    printf("  add \"Buy groceries\" high \"2024-01-15\"\n");
    printf("  list pending\n");
    printf("  complete 1234567890\n");
    printf("  search \"meeting\"\n");
    printf("  stats\n\n");

    printf(CYAN "Priorities:" RESET " high, medium, low (default: medium)\n");
    printf(CYAN "Date Format:" RESET " YYYY-MM-DD\n\n");
}

void clearScreen() {
    printf("\033[2J\033[H");
}

void printHeader() {
    printf(BRIGHT BLUE "🚀 Welcome to Todo CLI!" RESET "\n");
    printf(DIM "Type 'help' for available commands" RESET "\n\n");
}

void printPrompt() {
    printf(CYAN "todo> " RESET);
    fflush(stdout);
}

char* getPriorityColor(const char* priority) {
    if (strcmp(priority, "high") == 0) return RED;
    if (strcmp(priority, "medium") == 0) return YELLOW;
    if (strcmp(priority, "low") == 0) return GREEN;
    return WHITE;
}

int isValidDate(const char* date) {
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;

    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);

    return (year >= 2020 && year <= 2030 && month >= 1 && month <= 12 && day >= 1 && day <= 31);
}

void generateId(char* id) {
    sprintf(id, "%ld", time(NULL));
}

void getCurrentTime(char* timeStr) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M", tm_info);
}

void trim(char* str) {
    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) {
        *end = 0;
        end--;
    }
    while (*str && (*str == ' ' || *str == '\n' || *str == '\r')) {
        str++;
    }
}
