#include <stdio.h>
#include <stdlib.h>
#define ROWS 20
#define COLS 60
#define MAX_SHAPES 100
// Enumeration for shape types
typedef enum {
    SHAPE_LINE,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;
// Shape data structures
typedef struct {
    int x1, y1;
    int x2, y2;
} Line;
typedef struct {
    int x1, y1;
    int x2, y2;
} Rectangle;
typedef struct {
    int cx, cy;
    int r;
} Circle;
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} Triangle;
// Combined Shape structure
typedef struct {
    int id;
    ShapeType type;
    union {
        Line line;
        Rectangle rect;
        Circle circle;
        Triangle tri;
    } data;
} Shape;
// Global state
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_shape_id = 1;
char canvas[ROWS][COLS];
// Helper to read an integer with bounds checking and buffer flushing
int read_int(const char *prompt, int min_val, int max_val) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        char *endptr;
        val = (int)strtol(buffer, &endptr, 10);
        
        // Ensure some valid input was parsed and check for junk chars
        if (endptr == buffer || (*endptr != '\n' && *endptr != '\0')) {
            printf("Invalid input. Please enter a valid integer.\n");
            continue;
        }
        if (val < min_val || val > max_val) {
            printf("Value out of range [%d, %d]. Try again.\n", min_val, max_val);
            continue;
        }
        return val;
    }
}
// Clear the canvas by setting all pixels to background character '_'
void clear_canvas(char canv[ROWS][COLS]) {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            canv[y][x] = '_';
        }
    }
}
// Display the canvas with horizontal and vertical coordinate indices for usability
void display_canvas(char canv[ROWS][COLS]) {
    printf("\n");
    // Print column coordinate headers (tens place)
    printf("   ");
    for (int x = 0; x < COLS; x++) {
        if (x % 10 == 0) {
            printf("%d", x / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");
    // Print column coordinate headers (ones place)
    printf("   ");
    for (int x = 0; x < COLS; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
    // Print canvas rows with vertical indices
    for (int y = 0; y < ROWS; y++) {
        printf("%2d ", y);
        for (int x = 0; x < COLS; x++) {
            putchar(canv[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}
// Safe pixel plot function with boundary checks
void set_pixel(char canv[ROWS][COLS], int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canv[y][x] = '*';
    }
}
// Bresenham's line algorithm
void draw_line(char canv[ROWS][COLS], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;
    while (1) {
        set_pixel(canv, x1, y1);
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
// Rectangle drawn by drawing outline lines
void draw_rectangle(char canv[ROWS][COLS], int x1, int y1, int x2, int y2) {
    draw_line(canv, x1, y1, x2, y1); // Top
    draw_line(canv, x2, y1, x2, y2); // Right
    draw_line(canv, x2, y2, x1, y2); // Bottom
    draw_line(canv, x1, y2, x1, y1); // Left
}
// Midpoint circle algorithm
void draw_circle(char canv[ROWS][COLS], int cx, int cy, int r) {
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    while (y >= x) {
        set_pixel(canv, cx + x, cy + y);
        set_pixel(canv, cx - x, cy + y);
        set_pixel(canv, cx + x, cy - y);
        set_pixel(canv, cx - x, cy - y);
        set_pixel(canv, cx + y, cy + x);
        set_pixel(canv, cx - y, cy + x);
        set_pixel(canv, cx + y, cy - x);
        set_pixel(canv, cx - y, cy - x);
        
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}
// Triangle drawn by connecting 3 points
void draw_triangle(char canv[ROWS][COLS], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canv, x1, y1, x2, y2);
    draw_line(canv, x2, y2, x3, y3);
    draw_line(canv, x3, y3, x1, y1);
}
// Render shapes into the canvas array
void render_shapes(char canv[ROWS][COLS], Shape shp[], int count) {
    clear_canvas(canv);
    for (int i = 0; i < count; i++) {
        switch (shp[i].type) {
            case SHAPE_LINE:
                draw_line(canv, shp[i].data.line.x1, shp[i].data.line.y1, 
                                shp[i].data.line.x2, shp[i].data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(canv, shp[i].data.rect.x1, shp[i].data.rect.y1, 
                                     shp[i].data.rect.x2, shp[i].data.rect.y2);
                break;
            case SHAPE_CIRCLE:
                draw_circle(canv, shp[i].data.circle.cx, shp[i].data.circle.cy, 
                                  shp[i].data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(canv, shp[i].data.tri.x1, shp[i].data.tri.y1, 
                                    shp[i].data.tri.x2, shp[i].data.tri.y2, 
                                    shp[i].data.tri.x3, shp[i].data.tri.y3);
                break;
        }
    }
}
// Add a shape to the editor list
void add_shape_menu() {
    if (shape_count >= MAX_SHAPES) {
        printf("Error: Maximum shape limit reached (%d).\n", MAX_SHAPES);
        return;
    }
    printf("\nSelect Shape to Add:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("5. Back to Main Menu\n");
    
    int choice = read_int("Enter choice (1-5): ", 1, 5);
    if (choice == 5) return;
    Shape new_shape;
    new_shape.id = next_shape_id++;
    switch (choice) {
        case 1:
            printf("\n--- Add Line ---\n");
            new_shape.type = SHAPE_LINE;
            new_shape.data.line.x1 = read_int("Enter X1 (0-59): ", 0, COLS - 1);
            new_shape.data.line.y1 = read_int("Enter Y1 (0-19): ", 0, ROWS - 1);
            new_shape.data.line.x2 = read_int("Enter X2 (0-59): ", 0, COLS - 1);
            new_shape.data.line.y2 = read_int("Enter Y2 (0-19): ", 0, ROWS - 1);
            shapes[shape_count++] = new_shape;
            printf("Line added successfully (ID: %d).\n", new_shape.id);
            break;
        case 2:
            printf("\n--- Add Rectangle ---\n");
            new_shape.type = SHAPE_RECTANGLE;
            new_shape.data.rect.x1 = read_int("Enter Top-Left X (0-59): ", 0, COLS - 1);
            new_shape.data.rect.y1 = read_int("Enter Top-Left Y (0-19): ", 0, ROWS - 1);
            new_shape.data.rect.x2 = read_int("Enter Bottom-Right X (0-59): ", 0, COLS - 1);
            new_shape.data.rect.y2 = read_int("Enter Bottom-Right Y (0-19): ", 0, ROWS - 1);
            shapes[shape_count++] = new_shape;
            printf("Rectangle added successfully (ID: %d).\n", new_shape.id);
            break;
        case 3:
            printf("\n--- Add Circle ---\n");
            new_shape.type = SHAPE_CIRCLE;
            new_shape.data.circle.cx = read_int("Enter Center X (0-59): ", 0, COLS - 1);
            new_shape.data.circle.cy = read_int("Enter Center Y (0-19): ", 0, ROWS - 1);
            new_shape.data.circle.r = read_int("Enter Radius (0-30): ", 0, 30);
            shapes[shape_count++] = new_shape;
            printf("Circle added successfully (ID: %d).\n", new_shape.id);
            break;
        case 4:
            printf("\n--- Add Triangle ---\n");
            new_shape.type = SHAPE_TRIANGLE;
            new_shape.data.tri.x1 = read_int("Enter Vertex 1 X (0-59): ", 0, COLS - 1);
            new_shape.data.tri.y1 = read_int("Enter Vertex 1 Y (0-19): ", 0, ROWS - 1);
            new_shape.data.tri.x2 = read_int("Enter Vertex 2 X (0-59): ", 0, COLS - 1);
            new_shape.data.tri.y2 = read_int("Enter Vertex 2 Y (0-19): ", 0, ROWS - 1);
            new_shape.data.tri.x3 = read_int("Enter Vertex 3 X (0-59): ", 0, COLS - 1);
            new_shape.data.tri.y3 = read_int("Enter Vertex 3 Y (0-19): ", 0, ROWS - 1);
            shapes[shape_count++] = new_shape;
            printf("Triangle added successfully (ID: %d).\n", new_shape.id);
            break;
    }
}
// Delete a shape from the list
void delete_shape_menu() {
    if (shape_count == 0) {
        printf("\nNo objects to delete.\n");
        return;
    }
    printf("\n--- Active Objects ---\n");
    for (int i = 0; i < shape_count; i++) {
        printf("ID: %d | ", shapes[i].id);
        switch (shapes[i].type) {
            case SHAPE_LINE:
                printf("Line: (%d, %d) to (%d, %d)\n", 
                       shapes[i].data.line.x1, shapes[i].data.line.y1,
                       shapes[i].data.line.x2, shapes[i].data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("Rectangle: Top-Left (%d, %d), Bottom-Right (%d, %d)\n", 
                       shapes[i].data.rect.x1, shapes[i].data.rect.y1,
                       shapes[i].data.rect.x2, shapes[i].data.rect.y2);
                break;
            case SHAPE_CIRCLE:
                printf("Circle: Center (%d, %d), Radius %d\n", 
                       shapes[i].data.circle.cx, shapes[i].data.circle.cy,
                       shapes[i].data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("Triangle: A(%d, %d), B(%d, %d), C(%d, %d)\n", 
                       shapes[i].data.tri.x1, shapes[i].data.tri.y1,
                       shapes[i].data.tri.x2, shapes[i].data.tri.y2,
                       shapes[i].data.tri.x3, shapes[i].data.tri.y3);
                break;
        }
    }
    int id_to_delete = read_int("Enter the ID of the object to delete (or 0 to cancel): ", 0, 9999);
    if (id_to_delete == 0) return;
    int found_index = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id_to_delete) {
            found_index = i;
            break;
        }
    }
    if (found_index == -1) {
        printf("Object with ID %d not found.\n", id_to_delete);
        return;
    }
    // Shift shapes array to delete the item
    for (int i = found_index; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    printf("Object ID %d deleted successfully.\n", id_to_delete);
}
int main() {
    // Initialize canvas to empty background '_'
    clear_canvas(canvas);
    while (1) {
        printf("\n=== 2D Terminal Graphics Editor ===\n");
        printf("1. Display Canvas\n");
        printf("2. Add an Object (Line, Rectangle, Circle, Triangle)\n");
        printf("3. Delete an Object\n");
        printf("4. Clear All Objects\n");
        printf("5. Exit\n");
        int choice = read_int("Select option (1-5): ", 1, 5);
        switch (choice) {
            case 1:
                render_shapes(canvas, shapes, shape_count);
                display_canvas(canvas);
                break;
            case 2:
                add_shape_menu();
                break;
            case 3:
                delete_shape_menu();
                break;
            case 4:
                shape_count = 0;
                printf("All objects cleared.\n");
                break;
            case 5:
                printf("Exiting... Goodbye!\n");
                return 0;
        }
    }
}
