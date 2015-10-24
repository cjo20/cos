#include "keyboard.h"
#include "stdint.h"
#include "lib.h"
#include "mem.h"
#include "fb.h"
char prompt[32]  = ">";

#define BUFFER_LEN 512
char * read_command(char * cmd_buffer, uint16_t * cmd_length)
{
	char c;
	*cmd_length = 0;

	printf("%s", prompt);

	do
	{
		c = getc();
		if (c == KEY_BACKSPACE)
		{
			if (*cmd_length > 0)
			{
				fb_putch(c);
				cmd_buffer[*cmd_length - 1] = 0;
				*cmd_length = *cmd_length - 1;
			}
		}
		else if (*cmd_length == BUFFER_LEN)
		{
			continue;
		}
		else if (c != KEY_TAB 
				&& c != KEY_RETURN 
				&& c != KEY_UP
				&& c != KEY_DOWN
				&& c != KEY_RIGHT
				&& c != KEY_LEFT)
		{
			fb_putch(c);
			cmd_buffer[*cmd_length] = c;
			*cmd_length = *cmd_length + 1;
		}
	} while (c != KEY_RETURN);

	fb_putch('\n');

	return cmd_buffer;
}


uint8_t process_command(char * cmd_buffer, uint16_t cmd_length)
{
	char * cmd = cmd_buffer;
	char * args = cmd_buffer;

	uint16_t i;
	uint8_t status = 0;

	for (i = 0; args[0] != ' ' && args[0] != 0; i++,args++);
	*args = 0;
	if (i  < cmd_length)
	{
		args++;
	}

	if (strcmp(cmd, "help") == 0)
	{
		if (*args != 0)
		{
			printf("No such help available\n");	
		} 
		else
		{
			printf("You are running a really Simple-OS.\n");
		}
	}
	else if (strcmp(cmd, "cls") == 0)
	{
		fb_clear();
	}
	else if (strcmp(cmd, "shutdown") == 0)
	{
		printf("System halting\n");
		status = 0xFF;
	}
	else if (strcmp(cmd, "echo") == 0)
	{
		if (*args != 0)
		{
			printf("%s\n", args);
		}
		
	}
	else if (strcmp(cmd, "x") == 0)
	{
		if (*args != 0)
		{
			char * endPtr = args;
			while(*endPtr != ' ' && *endPtr != '\n')
			{
				endPtr++;
			}

			*endPtr = 0;
			unsigned long * addr = (unsigned long *)strtoul(args, 0, 0);
			printf("Value at %#08x: %#08x\n", addr, *addr);
		}


	}
	else if (cmd[0] != 0)
	{
		printf("%s: Command not found\n", cmd);
	}

	return status;
}


void start_console()
{
	char command_buffer[BUFFER_LEN + 1];
	uint16_t command_length;
	uint8_t status;
	

	while (1)
	{
		memset(command_buffer, 0, BUFFER_LEN);
		read_command(command_buffer, &command_length);
		status = process_command(command_buffer, command_length);
		if (status == 0xFF)
		{
			return;
		}
	}
}

