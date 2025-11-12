#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph_analysis.h"

typedef struct
{
    int* data;
    int top;
    int capacity;
} int_stack;

static void stack_init(int_stack* stack, int start_capacity)
{
    stack->top = -1;
    stack->capacity = (start_capacity > 0) ? start_capacity : 1;
    stack->data = (int*)malloc(stack->capacity * sizeof(int));
    if (stack->data == NULL)
    {
        printf("Error: cannot allocate memory for Tarjan stack\n");
        exit(EXIT_FAILURE);
    }
}

static void stack_push(int_stack* stack, int value)
{
    if (stack->top + 1 >= stack->capacity)
    {
        int new_capacity = stack->capacity * 2;
        int* new_data = (int*)realloc(stack->data, new_capacity * sizeof(int));
        if (new_data == NULL)
        {
            printf("Error: cannot grow Tarjan stack\n");
            exit(EXIT_FAILURE);
        }
        stack->data = new_data;
        stack->capacity = new_capacity;
    }
    stack->top++;
    stack->data[stack->top] = value;
}

static int stack_pop(int_stack* stack)
{
    if (stack->top < 0)
    {
        return -1;
    }
    int value = stack->data[stack->top];
    stack->top--;
    return value;
}

static void stack_free(int_stack* stack)
{
    free(stack->data);
    stack->data = NULL;
    stack->top = -1;
    stack->capacity = 0;
}

static void init_partition(t_partition* partition)
{
    partition->class_count = 0;
    partition->capacity = 4;
    partition->classes = (t_class*)malloc(partition->capacity * sizeof(t_class));
    if (partition->classes == NULL)
    {
        printf("Error: cannot allocate memory for partition\n");
        exit(EXIT_FAILURE);
    }
}

static void ensure_partition_capacity(t_partition* partition)
{
    if (partition->class_count >= partition->capacity)
    {
        int new_capacity = partition->capacity * 2;
        t_class* new_classes = (t_class*)realloc(partition->classes, new_capacity * sizeof(t_class));
        if (new_classes == NULL)
        {
            printf("Error: cannot grow partition\n");
            exit(EXIT_FAILURE);
        }
        partition->classes = new_classes;
        partition->capacity = new_capacity;
    }
}

static void ensure_class_capacity(t_class* cls)
{
    if (cls->member_count >= cls->capacity)
    {
        int new_capacity = cls->capacity * 2;
        int* new_members = (int*)realloc(cls->members, new_capacity * sizeof(int));
        if (new_members == NULL)
        {
            printf("Error: cannot grow class members\n");
            exit(EXIT_FAILURE);
        }
        cls->members = new_members;
        cls->capacity = new_capacity;
    }
}

static int create_class(t_partition* partition)
{
    ensure_partition_capacity(partition);

    t_class* cls = &partition->classes[partition->class_count];
    cls->member_count = 0;
    cls->capacity = 4;
    cls->members = (int*)malloc(cls->capacity * sizeof(int));
    if (cls->members == NULL)
    {
        printf("Error: cannot allocate class members\n");
        exit(EXIT_FAILURE);
    }
    snprintf(cls->name, sizeof(cls->name), "C%d", partition->class_count + 1);
    partition->class_count++;
    return partition->class_count - 1;
}

static void add_member_to_class(t_class* cls, int vertex_number)
{
    ensure_class_capacity(cls);
    cls->members[cls->member_count] = vertex_number;
    cls->member_count++;
}

static void sort_class_members(t_class* cls)
{
    for (int i = 0; i < cls->member_count - 1; i++)
    {
        for (int j = i + 1; j < cls->member_count; j++)
        {
            if (cls->members[j] < cls->members[i])
            {
                int temp = cls->members[i];
                cls->members[i] = cls->members[j];
                cls->members[j] = temp;
            }
        }
    }
}

static void tarjan_visit(int vertex_index,
                         const adjacency_list* graph,
                         t_tarjan_vertex* vertices,
                         int_stack* stack,
                         t_partition* partition,
                         int* current_index,
                         int* vertex_to_class)
{
    t_tarjan_vertex* vertex = &vertices[vertex_index];
    vertex->index = *current_index;
    vertex->low_link = *current_index;
    (*current_index)++;

    stack_push(stack, vertex_index);
    vertex->on_stack = 1;

    cell* current = graph->lists[vertex_index].head;
    while (current != NULL)
    {
        int neighbour_index = current->arrival_vertex - 1;
        if (vertices[neighbour_index].index == -1)
        {
            tarjan_visit(neighbour_index, graph, vertices, stack, partition, current_index, vertex_to_class);
            if (vertices[neighbour_index].low_link < vertex->low_link)
            {
                vertex->low_link = vertices[neighbour_index].low_link;
            }
        }
        else if (vertices[neighbour_index].on_stack)
        {
            if (vertices[neighbour_index].index < vertex->low_link)
            {
                vertex->low_link = vertices[neighbour_index].index;
            }
        }
        current = current->next;
    }

    if (vertex->low_link == vertex->index)
    {
        int class_index = create_class(partition);
        int popped;
        do
        {
            popped = stack_pop(stack);
            vertices[popped].on_stack = 0;
            add_member_to_class(&partition->classes[class_index], popped + 1);
            vertex_to_class[popped] = class_index;
        } while (popped != vertex_index);

        sort_class_members(&partition->classes[class_index]);
    }
}

t_partition tarjan_partition_graph(const adjacency_list* graph, int** vertex_to_class)
{
    t_partition partition;
    init_partition(&partition);

    int vertex_count = graph->num_vertices;
    t_tarjan_vertex* vertices = (t_tarjan_vertex*)malloc(vertex_count * sizeof(t_tarjan_vertex));
    if (vertices == NULL)
    {
        printf("Error: cannot allocate Tarjan vertices\n");
        exit(EXIT_FAILURE);
    }

    *vertex_to_class = (int*)malloc(vertex_count * sizeof(int));
    if (*vertex_to_class == NULL)
    {
        printf("Error: cannot allocate vertex-to-class array\n");
        free(vertices);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < vertex_count; i++)
    {
        vertices[i].identifier = i + 1;
        vertices[i].index = -1;
        vertices[i].low_link = -1;
        vertices[i].on_stack = 0;
        (*vertex_to_class)[i] = -1;
    }

    int_stack stack;
    stack_init(&stack, vertex_count);
    int current_index = 0;

    for (int i = 0; i < vertex_count; i++)
    {
        if (vertices[i].index == -1)
        {
            tarjan_visit(i, graph, vertices, &stack, &partition, &current_index, *vertex_to_class);
        }
    }

    stack_free(&stack);
    free(vertices);

    return partition;
}

void print_partition(const t_partition* partition)
{
    printf("Strongly connected components:\n");
    for (int i = 0; i < partition->class_count; i++)
    {
        const t_class* cls = &partition->classes[i];
        printf("Component %s: {", cls->name);
        for (int j = 0; j < cls->member_count; j++)
        {
            printf("%d", cls->members[j]);
            if (j < cls->member_count - 1)
            {
                printf(", ");
            }
        }
        printf("}\n");
    }
    printf("\n");
}

void free_partition(t_partition* partition)
{
    if (partition->classes == NULL)
    {
        return;
    }
    for (int i = 0; i < partition->class_count; i++)
    {
        free(partition->classes[i].members);
        partition->classes[i].members = NULL;
        partition->classes[i].member_count = 0;
        partition->classes[i].capacity = 0;
    }
    free(partition->classes);
    partition->classes = NULL;
    partition->class_count = 0;
    partition->capacity = 0;
}

static void ensure_link_capacity(t_link_array* link_array)
{
    if (link_array->size >= link_array->capacity)
    {
        int new_capacity = link_array->capacity * 2;
        t_link* new_links = (t_link*)realloc(link_array->links, new_capacity * sizeof(t_link));
        if (new_links == NULL)
        {
            printf("Error: cannot grow link array\n");
            exit(EXIT_FAILURE);
        }
        link_array->links = new_links;
        link_array->capacity = new_capacity;
    }
}

static int link_exists(const t_link_array* link_array, int from, int to)
{
    for (int i = 0; i < link_array->size; i++)
    {
        if (link_array->links[i].from == from && link_array->links[i].to == to)
        {
            return 1;
        }
    }
    return 0;
}

t_link_array build_link_array(const t_partition* partition, const adjacency_list* graph, const int* vertex_to_class)
{
    (void)partition;

    t_link_array link_array;
    link_array.size = 0;
    link_array.capacity = 8;
    link_array.links = (t_link*)malloc(link_array.capacity * sizeof(t_link));
    if (link_array.links == NULL)
    {
        printf("Error: cannot allocate link array\n");
        exit(EXIT_FAILURE);
    }

    for (int vertex = 0; vertex < graph->num_vertices; vertex++)
    {
        int class_from = vertex_to_class[vertex];
        cell* current = graph->lists[vertex].head;
        while (current != NULL)
        {
            int neighbour = current->arrival_vertex - 1;
            int class_to = vertex_to_class[neighbour];
            if (class_from != class_to)
            {
                if (!link_exists(&link_array, class_from, class_to))
                {
                    ensure_link_capacity(&link_array);
                    link_array.links[link_array.size].from = class_from;
                    link_array.links[link_array.size].to = class_to;
                    link_array.size++;
                }
            }
            current = current->next;
        }
    }

    return link_array;
}

t_link_array clone_link_array(const t_link_array* source)
{
    t_link_array clone;
    clone.size = source->size;
    clone.capacity = source->capacity;
    clone.links = (t_link*)malloc(clone.capacity * sizeof(t_link));
    if (clone.links == NULL)
    {
        printf("Error: cannot clone link array\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < source->size; i++)
    {
        clone.links[i] = source->links[i];
    }
    return clone;
}

void free_link_array(t_link_array* link_array)
{
    free(link_array->links);
    link_array->links = NULL;
    link_array->size = 0;
    link_array->capacity = 0;
}

void print_link_array(const t_link_array* link_array, const t_partition* partition)
{
    if (link_array->size == 0)
    {
        printf("No links between classes (all classes are closed).\n\n");
        return;
    }

    printf("Links between classes:\n");
    for (int i = 0; i < link_array->size; i++)
    {
        int from = link_array->links[i].from;
        int to = link_array->links[i].to;
        printf("%s -> %s\n",
               partition->classes[from].name,
               partition->classes[to].name);
    }
    printf("\n");
}

void export_hasse_mermaid(const t_partition* partition, const t_link_array* link_array, const char* filename)
{
    FILE* file = fopen(filename, "wt");
    if (file == NULL)
    {
        printf("Error: cannot open '%s' for writing.\n", filename);
        return;
    }

    fprintf(file, "flowchart LR\n");
    for (int i = 0; i < partition->class_count; i++)
    {
        const t_class* cls = &partition->classes[i];
        fprintf(file, "%s[\"%s {", cls->name, cls->name);
        for (int j = 0; j < cls->member_count; j++)
        {
            fprintf(file, "%d", cls->members[j]);
            if (j < cls->member_count - 1)
            {
                fprintf(file, ",");
            }
        }
        fprintf(file, "}\"]\n");
    }

    for (int i = 0; i < link_array->size; i++)
    {
        int from = link_array->links[i].from;
        int to = link_array->links[i].to;
        fprintf(file, "%s --> %s\n",
                partition->classes[from].name,
                partition->classes[to].name);
    }

    fclose(file);
    printf("Hasse diagram saved in '%s'\n", filename);
}

graph_characteristics compute_graph_characteristics(const t_partition* partition, const t_link_array* link_array)
{
    graph_characteristics characteristics;
    characteristics.class_is_persistent = (int*)malloc(partition->class_count * sizeof(int));
    if (characteristics.class_is_persistent == NULL)
    {
        printf("Error: cannot allocate characteristics array\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < partition->class_count; i++)
    {
        characteristics.class_is_persistent[i] = 1;
    }

    for (int i = 0; i < link_array->size; i++)
    {
        int from = link_array->links[i].from;
        characteristics.class_is_persistent[from] = 0;
    }

    characteristics.has_absorbing_state = 0;
    for (int i = 0; i < partition->class_count; i++)
    {
        if (characteristics.class_is_persistent[i] && partition->classes[i].member_count == 1)
        {
            characteristics.has_absorbing_state = 1;
        }
    }

    characteristics.is_irreducible = (partition->class_count == 1);

    return characteristics;
}

void print_graph_characteristics(const t_partition* partition, const graph_characteristics* characteristics)
{
    printf("Class properties:\n");
    for (int i = 0; i < partition->class_count; i++)
    {
        printf("- %s is %s\n",
               partition->classes[i].name,
               characteristics->class_is_persistent[i] ? "persistent" : "transient");
    }
    printf("\n");

    if (characteristics->has_absorbing_state)
    {
        printf("Absorbing states:\n");
        for (int i = 0; i < partition->class_count; i++)
        {
            if (characteristics->class_is_persistent[i] && partition->classes[i].member_count == 1)
            {
                printf("* State %d (class %s)\n",
                       partition->classes[i].members[0],
                       partition->classes[i].name);
            }
        }
    }
    else
    {
        printf("No absorbing states found.\n");
    }
    printf("\n");

    if (characteristics->is_irreducible)
    {
        printf("The Markov graph is irreducible (only one class).\n");
    }
    else
    {
        printf("The Markov graph is not irreducible (more than one class).\n");
    }
    printf("\n");
}

void free_graph_characteristics(graph_characteristics* characteristics)
{
    free(characteristics->class_is_persistent);
    characteristics->class_is_persistent = NULL;
    characteristics->has_absorbing_state = 0;
    characteristics->is_irreducible = 0;
}

