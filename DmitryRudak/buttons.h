#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

int AddButton(char *name, float x, float y, float width, float height, float textScale);
void ShowMenu();

int MouseMove(float x, float y);
int MouseDown();
int MouseUp();
char *GetButtonName(int buttonId);


#endif // BUTTONS_H_INCLUDED
