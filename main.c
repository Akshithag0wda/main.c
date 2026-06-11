#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 20
#define WIDTH 60
#define MAX_SHAPES 100

// Shape types enumeration
typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

// Parameter structures for each shape
typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct {
    int x1, y1;
    int x2, y2;
} RectParams;

typedef struct {
    int xc, yc;
    int r;
} CircleParams;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleParams;

// Combined Shape structure incorporating unique ID
typedef struct {
    int id;
    ShapeType type;
    union {
        LineParams line;
        RectParams rect;
        CircleParams circle;
        TriangleParams triangle;
    } params;
} Shape;

// Global state
Shape shapes[MAX_SHAPES];
int num_shapes = 0;
int next_id = 1; // Unique ID generator
char canvas[HEIGHT][WIDTH];

// Get standard integer input within bounds using fgets and strtol
int get_int_input(const char* prompt, int min_val, int max_val) {
    char buffer[100];
    char *endptr;
    long val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            val = strtol(buffer, &endptr, 10);
            
            // Check if parsing succeeded
            if (endptr == buffer) {
                printf("Invalid input! Please enter an integer.\n");
                continue;
            }
            
            // Skip trailing whitespace
            while (*endptr == ' ' || *endptr == '\t' || *endptr == '\r' || *endptr == '\n') {
                endptr++;
            }
            
            if (*endptr != '\0') {
                printf("Invalid input! Please enter a clean integer.\n");
                continue;
            }

            if (val >= min_val && val <= max_val) {
                return (int)val;
            }
            printf("Invalid range! Please enter a value between %d and %d.\n", min_val, max_val);
        } else {
            printf("Error reading input or EOF reached.\n");
            exit(1);
        }
    }
}

// Canvas manipulation functions: initializes canvas with '_'
void clear_canvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Safely draw pixel inside boundaries
void set_pixel(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        set_pixel(canvas, x1, y1);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draw rectangle outlines using four lines connecting opposite corners
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    draw_line(canvas, x1, y1, x2, y1); // Top edge
    draw_line(canvas, x2, y1, x2, y2); // Right edge
    draw_line(canvas, x2, y2, x1, y2); // Bottom edge
    draw_line(canvas, x1, y2, x1, y1); // Left edge
}

// Draw 8 symmetric points for standard Circle Algorithm
void draw_circle_points(char canvas[HEIGHT][WIDTH], int xc, int yc, int x, int y) {
    set_pixel(canvas, xc + x, yc + y);
    set_pixel(canvas, xc - x, yc + y);
    set_pixel(canvas, xc + x, yc - y);
    set_pixel(canvas, xc - x, yc - y);
    set_pixel(canvas, xc + y, yc + x);
    set_pixel(canvas, xc - y, yc + x);
    set_pixel(canvas, xc + y, yc - x);
    set_pixel(canvas, xc - y, yc - x);
}

// Bresenham's Midpoint Circle Algorithm
void draw_circle(char canvas[HEIGHT][WIDTH], int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    draw_circle_points(canvas, xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        draw_circle_points(canvas, xc, yc, x, y);
    }
}

// Draw triangle from 3 vertices using three lines
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}

// Re-render all active shapes on clean canvas
void render_shapes() {
    clear_canvas(canvas);
    for (int i = 0; i < num_shapes; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(canvas, s.params.line.x1, s.params.line.y1, s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canvas, s.params.rect.x1, s.params.rect.y1, s.params.rect.x2, s.params.rect.y2);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canvas, s.params.circle.xc, s.params.circle.yc, s.params.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canvas, s.params.triangle.x1, s.params.triangle.y1,
                                      s.params.triangle.x2, s.params.triangle.y2,
                                      s.params.triangle.x3, s.params.triangle.y3);
                break;
        }
    }
}

// List all active shapes in the picture
void list_shapes() {
    if (num_shapes == 0) {
        printf("No active shapes in the picture.\n");
        return;
    }
    printf("Active Shapes in the picture:\n");
    for (int i = 0; i < num_shapes; i++) {
        Shape s = shapes[i];
        printf("ID [%d] - ", s.id);
        switch (s.type) {
            case SHAPE_LINE:
                printf("Line: (%d, %d) to (%d, %d)\n", 
                       s.params.line.x1, s.params.line.y1, 
                       s.params.line.x2, s.params.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle: corners (%d, %d) and (%d, %d)\n", 
                       s.params.rect.x1, s.params.rect.y1, 
                       s.params.rect.x2, s.params.rect.y2);
                break;
            case SHAPE_CIRCLE:
                printf("Circle: center (%d, %d), radius %d\n", 
                       s.params.circle.xc, s.params.circle.yc, 
                       s.params.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle: vertices (%d, %d), (%d, %d), (%d, %d)\n", 
                       s.params.triangle.x1, s.params.triangle.y1, 
                       s.params.triangle.x2, s.params.triangle.y2, 
                       s.params.triangle.x3, s.params.triangle.y3);
                break;
        }
    }
}

// Print canvas grid with horizontal and vertical headers for easy coordinate tracing
void display_canvas(char canvas[HEIGHT][WIDTH]) {
    printf("\n");
    // Top border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");

    for (int y = 0; y < HEIGHT; y++) {
        printf("%2d |", y);
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", canvas[y][x]);
        }
        printf("|\n");
    }

    // Bottom border
    printf("   +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
    
    // Draw column numbers (tens)
    printf("    ");
    for (int x = 0; x < WIDTH; x++) {
        if (x % 10 == 0) printf("%d", x / 10);
        else printf(" ");
    }
    printf("\n    ");
    // Draw column numbers (units)
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n\n");

    list_shapes();
}

// Menu handler for adding shapes
void add_shape_menu() {
    if (num_shapes >= MAX_SHAPES) {
        printf("Error: Maximum shape capacity reached (%d)!\n", MAX_SHAPES);
        return;
    }

    printf("\n--- Add Object Menu ---\n");
    printf("1. Add Line\n");
    printf("2. Add Rectangle\n");
    printf("3. Add Circle\n");
    printf("4. Add Triangle\n");
    printf("5. Back to Main Menu\n");
    
    int choice = get_int_input("Choose shape to add: ", 1, 5);
    if (choice == 5) return;

    Shape s;
    s.id = next_id++; // Assign unique ID
    
    switch (choice) {
        case 1: // Line
            s.type = SHAPE_LINE;
            printf("Enter details for the Line:\n");
            s.params.line.x1 = get_int_input("Start X (0 to 59): ", 0, WIDTH - 1);
            s.params.line.y1 = get_int_input("Start Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.line.x2 = get_int_input("End X (0 to 59): ", 0, WIDTH - 1);
            s.params.line.y2 = get_int_input("End Y (0 to 19): ", 0, HEIGHT - 1);
            break;
        case 2: // Rectangle
            s.type = SHAPE_RECTANGLE;
            printf("Enter details for the Rectangle (two opposite corner vertices):\n");
            s.params.rect.x1 = get_int_input("Corner 1 X (0 to 59): ", 0, WIDTH - 1);
            s.params.rect.y1 = get_int_input("Corner 1 Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.rect.x2 = get_int_input("Corner 2 X (0 to 59): ", 0, WIDTH - 1);
            s.params.rect.y2 = get_int_input("Corner 2 Y (0 to 19): ", 0, HEIGHT - 1);
            break;
        case 3: // Circle
            s.type = SHAPE_CIRCLE;
            printf("Enter details for the Circle:\n");
            s.params.circle.xc = get_int_input("Center X (0 to 59): ", 0, WIDTH - 1);
            s.params.circle.yc = get_int_input("Center Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.circle.r  = get_int_input("Radius (1 to 30): ", 1, 30);
            break;
        case 4: // Triangle
            s.type = SHAPE_TRIANGLE;
            printf("Enter details for the Triangle (3 vertices):\n");
            s.params.triangle.x1 = get_int_input("Vertex 1 X (0 to 59): ", 0, WIDTH - 1);
            s.params.triangle.y1 = get_int_input("Vertex 1 Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.triangle.x2 = get_int_input("Vertex 2 X (0 to 59): ", 0, WIDTH - 1);
            s.params.triangle.y2 = get_int_input("Vertex 2 Y (0 to 19): ", 0, HEIGHT - 1);
            s.params.triangle.x3 = get_int_input("Vertex 3 X (0 to 59): ", 0, WIDTH - 1);
            s.params.triangle.y3 = get_int_input("Vertex 3 Y (0 to 19): ", 0, HEIGHT - 1);
            break;
    }

    shapes[num_shapes++] = s;
    printf("Shape added successfully with ID %d!\n", s.id);
    render_shapes();
}

// Menu handler for deleting shapes using their unique ID
void delete_shape_menu() {
    if (num_shapes == 0) {
        printf("No shapes to delete.\n");
        return;
    }

    list_shapes();
    int target_id = get_int_input("Enter the ID of the shape to delete (or 0 to cancel): ", 0, 99999);
    if (target_id == 0) return;

    int found_index = -1;
    for (int i = 0; i < num_shapes; i++) {
        if (shapes[i].id == target_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Error: Shape with ID %d not found!\n", target_id);
        return;
    }

    // Shift shapes to fill the gap
    for (int i = found_index; i < num_shapes - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    num_shapes--;
    printf("Shape with ID %d deleted successfully!\n", target_id);
    render_shapes();
}

int main() {
    // Initial render of empty canvas
    render_shapes();

    printf("=========================================\n");
    printf("       2D Graphics Editor (Console)      \n");
    printf("=========================================\n");

    while (1) {
        printf("\n--- Main Menu ---\n");
        printf("1. Display Canvas\n");
        printf("2. Add Object\n");
        printf("3. Delete Object\n");
        printf("4. Clear All Objects\n");
        printf("5. Exit\n");

        int choice = get_int_input("Enter your choice: ", 1, 5);

        switch (choice) {
            case 1:
                display_canvas(canvas);
                break;
            case 2:
                add_shape_menu();
                break;
            case 3:
                delete_shape_menu();
                break;
            case 4:
                num_shapes = 0;
                render_shapes();
                printf("All objects cleared successfully.\n");
                break;
            case 5:
                printf("Exiting 2D Graphics Editor. Goodbye!\n");
                return 0;
        }
    }
}
