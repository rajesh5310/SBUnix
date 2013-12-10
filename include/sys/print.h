/* @name : print.h
 * @author : rgolani, abmishra, skandalamsri
 * @last updated date : 25th September, 2013
 */

#ifndef WHATEVER_H_INCLUDED
#define WHATEVER_H_INCLUDED
void clear_screen();
int print_line(int x, int y, char *message, ...);
int print(char *message, ...);
void print_backspace();
void update_cursor(int row, int col);
void update_cursor_current_loc();
void change_video_pointer();
void puts(char *str);
void panic(char *message);
int putc(char ch);
int putc_color(char ch);


#endif
