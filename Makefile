SRCS	= starburst.c sparkle.c burst.c

OBJS	= ${SRCS:.c=.o}

NAME	= starburst

CC		= cc

CFLAGS	= -Wall -Wextra -Werror -O2

LFLAGS	= -lm

RM		= rm -f

all:	${NAME}

.c.o:
	${CC} ${CFLAGS} -c $< -o ${<:.c=.o}

${NAME}:	${OBJS}
	${CC} ${OBJS} ${LFLAGS} -o ${NAME}

clean:
	${RM} ${OBJS}

fclean: clean
	${RM} ${NAME}

re:	fclean all

.PHONY:	all clean fclean re
