#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void terminal_init();

void terminal_set_cursor_position(int x, int y);
void terminal_clear();
void terminal_putchar(char c);