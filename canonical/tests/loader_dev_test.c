#include <assert.h>
#include <stddef.h>

void bzero(void *s, size_t n);
int tolower(int c);
int toupper(int c);
void *memcpy(void *dest, void *src, size_t n);
char *strncpy(char *dest, char *src, size_t n);
size_t strnlen(char *string, size_t maxlen);

int main(void)
{
    unsigned char bytes[] = {0x11, 0x22, 0x33};
    unsigned char source[] = {0xa1, 0xb2, 0xc3};
    unsigned char copied[] = {0, 0, 0};
    char destination[6] = {'?', '?', '?', '?', '?', '?'};

    bzero(bytes, 3);
    /* The reference code writes the same byte once per iteration. */
    assert(bytes[0] == 0 && bytes[1] == 0x22 && bytes[2] == 0x33);

    assert(strnlen("abc", 8) == 3);
    assert(strnlen("abc", 2) == 2);
    assert(strncpy(destination, "xy", sizeof(destination)) == destination);
    assert(destination[0] == 'x' && destination[1] == 'y' && destination[2] == '\0');

    assert(memcpy(copied, source, sizeof(source)) == copied);
    assert(copied[0] == 0xa1 && copied[1] == 0xb2 && copied[2] == 0xc3);
    assert(toupper('a') == 'A' && toupper('!') == '!');
    assert(tolower('Z') == 'z' && tolower('!') == '!');
    return 0;
}
