/*
Введение

Одной из важных задач любой программы, будь она консольной или графической
является интерпретация аргументов командной строки. Формально аргументами
называются все слова в командной строке(в том числе и имя самой команды)
разбитые разделителем (как правило, это пробел и табуляция), кавычки же
позволяют включать разделители в аргументы.

Аргументы можно подразделить на опции и операнды. Опции изменяют поведение
программы или предоставляют ей дополнительную информацию. У опции могут
быть свои аргументы, которые являются информацией необходимой только для
этой опции.

POSIX

Стандарт POSIX описывает ожидаемое поведение программ в UNIX-подобных
системах. При написание программы никто вас не заставит строго следовать
стандарту, однако, это является хорошей идеей, так как это облегчит жизнь
вашим пользователям. Вот основные правила касающиеся аргументов команды:

в имени программы должно быть не менее 2 и не более 9 символов;
- имена программ должны быть написаны только строчными символами и цифрами;
- имя опции должно быть простым буквенно-цифровым символом. Опции с множеством
  цифр запрещены;
- все опции должны начинаться с символа "-";
- для опций без аргументов должны быть реализована возможность объединения
  опций (например foo -a -b -c и foo -abc);
- аргумент опции должен отделятся от нее пробелом;
- аргумент опции не может быть необязательным;
- если опции требуется множество значений аргумента, они должны передаваться
  в виде строки, разделенные запятыми или разделителем;
- опции должны идти перед операндами;
- аргумент "--" указывает на окончание всех опций;
- порядок опций не должен играть роли, кроме случаев когда опции взаимоисключающие,
  тогда побеждает последняя;
- порядок аргументов может иметь значение;
- программы читающие или записывающие именованные файлы, должны трактовать
  единственный аргумент "-" как стандартный ввод или стандартный вывод
  соответственно.

Длинные опции

В GNU программах также используются длинные опции, поведение которых не
описано в POSIX, длинные опции начинаются с "--" Для этих опций в GNU
также реализованы следующие соглашения:

- каждая короткая опция должна иметь свой вариант длинной опции;
- длинную опцию можно сократить до кратчайшей строки, обеспечивающей ее
  уникальность;
- Аргументы длинной опции отделяются либо разделителем, либо знаком "=".

Откуда берутся параметры в программе

как известно функция main() в С определяется так:

int main(int argc, char *argv[])

Здесь присутствует два параметра: argc определяет количество аргументов
в командной строке, а argv хранит массив указателей на эти аргументы.

Следует отметить, что argv[0] — всегда имя команды, а argv[argc] == NULL,
эти два факта могут оказаться полезными при разработке.

Разбор опций

В 80-х годах группа поддержки Unix заметила, что каждая программа Unix
использует собственные методы разбора опций. Что послужило толчком к
разработке функции getopt(), чтобы упростить написание кода, придерживающегося
стандартных соглашений.

Функция GNU getopt_long(), является совместимой с getopt(), а также упрощает
разбор длинных опций.

getopt

Объявление:

#include <unistd.h>
int getopt(int argc, char *argv[], const char *optstring);
extern char *optarg;
extern int optind, opterr, optopt;

Аргументы argc и argv передаются непосредственно от функции main(), а
optstring является строкой символов опций. Если за какой либо буквой в
строке следует двоеточие, значит эта опция принимает аргумент.

Для использования getopt() ее вызывают повторно в цикле, до тех пор, пока
она не вернет -1. Каждый раз обнаружив действительный символ опции, функция
возвращает этот символ. Если опция принимает аргумент, то указатель на
него помещается в переменную optarg.

переменная optind хранит текущий индекс в argv. Когда переменная opterr
не равна нулю (по умолчанию 0), getopt() сама выводит сообщения в случае
недействительной опции или отсутствия аргумента. Если же opterr равен
нулю, то в случае возникновения ошибки getopt() возвращает "?" или ":"
в зависимости от того найдена недействительная опция или пропущен обязательный
аргумент опции, в переменной optopt будет находится обнаруженный недействительный
символ.

Следует заметить, что стандартная функция getopt() останавливается сразу
как только найдет первый аргумент начинающийся не с символа "-", GNU вариант
функции просматривает в поисках опций, всю командную строку. Поведение
GNU функции можно изменить (но это выходит за рамки статьи).

пример программы с использованием getopt()

Любезно предоставил iv_s

		#include <unistd.h>
		#include <stdlib.h>
		#include <stdio.h>

		int main(int argc, char **argv) {
			if(argc == 1) { // если запускаем без аргументов, выводим справку
				printf(«getopt test\n»);
				printf(«usage:\n»);
				printf(" opts -a n -b m -o s\n");
				printf(«example:\n»);
				printf(" $ opts -a 323 -b 23 -o '-'\n");
				printf(" 323 — 23 = 300\n");
				return 0;
			}
			char *opts = «a:b:o:»; // доступные опции, каждая принимает аргумент
			int a, b; // тут храним числа
			char op; // а тут оператор
			int opt; // каждая следующая опция попадает сюда
			while((opt = getopt(argc, argv, opts)) != -1) { // вызываем getopt пока она не вернет -1
				switch(opt) {
					case 'a': // если опция -a, преобразуем строку с аргументом в число
						a = atoi(optarg);
						break;
					 case 'b': // тоже для -b
						 b = atoi(optarg);
						 break;
					 case 'o': // в op сохраняем оператор
						 op = optarg[0];
						 break;
				}
			}
			switch(op) {
				case '+': // если опаратор + складываем, и т.д.
					printf("%d + %d = %d\n", a, b, a + b);
					break;
				case '-':
					printf("%d — %d = %d\n", a, b, a — b);
					break;
				case '*':
					printf("%d * %d = %d\n", a, b, a * b);
					break;
				case '/':
					printf("%d / %d = %d\n", a, b, a / b);
					break;
			}
			return 0;
		}

	* This source code was highlighted with Source Code Highlighter.

getopt_long()

Объявление:
#include <getopt.h>

int getopt_long(int argc, char *argv[], const char *optstring, const struct option *longopts, int *longindex);

первые три аргумента те же, что и в getopt(), longopts является указателем
на массив длинных опций, longindex указывает на переменную, в которую
помещается индекс обнаруженной длинной опции в longopts, если в это нет
необходимости может быть NULL.

Структура option определена следующим образом:
struct option
{
const char *name;
int has_arg;
int *flag;
int val;
}

name — имя опции без предшествующих черточек;
has_arg — как понятно из названия, переменная описывает имеет ли длинная
опция аргумент, может принимать три значения:

	0 — не принимает аргумент;
	1 — обязательный аргумент;
	2 — необязательный аргумент.


flag — если этот указатель равен NULL, то getopt_long() возвращает значение
поля val, иначе она возвращает 0, а переменная на которую указывает flag
заполняется значением val;

val — обычно содержит некоторую символьную константу, если длинная опция
соответствует короткой, то эта константа должна быть такой же как и та
что появляется в аргументе optstring.

Важно заметить, что последний элемент массива longopts, должен быть заполнен нулями.

Пример программы с использованием getopt_long()

Любезно предоставил shuffle

		#include <stdio.h>
		#include <stdlib.h>
		#include <unistd.h>
		#include <getopt.h>

		void usage(char *name)
		{
		printf(«usage: %s\n \     \t-h this message\n \     \t-c [config file]\n \     \t--help this message\n \     \t--config=config_file\n», name);
		return;
		}

		int main (int argc, char *argv[])
		{
		int c;
		while (1){
		 static struct option long_opt[] = {
							{«help», 0, 0, 'h'},
							{«config», 1, 0, 'c'},
							{0,0,0,0}
						  };
		 int optIdx;

		 if((c = getopt_long(argc, argv, «c:h», long_opt, &optIdx)) == -1)
		  break;
		 switch( c ){
			 case 'h':
				 usage(argv[0]);
				 return(-1);

			 case 'c':
				 printf(«option 'c' selected, filename: %s\n», optarg);
				 return(1);

			 default:
				 usage(argv[0]);
				 return(-1);
		  }
		}

		return(0);
		}

	* This source code was highlighted with Source Code Highlighter.

Заключение

В статье рассмотрены основы использования функций разбора аргументов командной
строки в UNIX-подобных системах. Этот материал может быть более обширен,
но любой заинтересовавшийся человек в состоянии узнать и изучить все
тонкости самостоятельно.


Статья подготовлена по материалам книги Арнольда Роббинса «Linux программирование в примерах» ISBN 5-9579-0059-1

https://habrahabr.ru/post/55665/

*/

/*
…or create a new repository on the command line

echo "# vdd" >> README.md
git init
git add README.md
git commit -m "first commit"
git remote add origin https://github.com/kaljan/vdd.git
git push -u origin master

…or push an existing repository from the command line

git remote add origin https://github.com/kaljan/vdd.git
git push -u origin master

…or import code from another repository

You can initialize this repository with code from a Subversion, Mercurial, or TFS project.

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct _t_input_arg {
	char *arg;
} t_input_arg;


/*
 * Структура аргумента, делается просто. пишутся функции, которые
 * обрабатывают конкретный параметр или аргумент, затем создаётся
 * массив, в котором все эти функции обрабатывают аргумент.
 */
typedef struct _t_opt_arg {
	char *arg;
	int (*handler)(char *param);
} t_opt_arg;

t_input_arg input_args[12] = {
	{"-f\0"},
	{"--from=\0"},
	{"-t\0"},
	{"--to=\0"},
	{"-b\0"},
	{"--block-size=\0"},
	{"-c\0"},
	{"--count=\0"},
	{"-v\0"},
	{"--verbose\0"},
	{"--help\0"},
	{NULL},

};

/*
-f --from=[FILE] - копировать из файла
-t --to=[FILE] - копировать в файл
-b --block-size=[SIZE] - размер блока
-c --count=[NUM] - количество
-v --verbose - показывать процесс
*/

int get_file_size(char * fname, size_t *s)
{
	FILE *fp;
	struct stat fp_stat;

	if (NULL == (fp = fopen(fname, "r"))) {
		printf("Can\'t open file %s. error: %s\n", fname, strerror(errno));
		return -1;
	}

	if (fstat(fileno(fp), &fp_stat) != 0) {
		printf("fstat failed: %s\n", strerror(errno));
		fclose(fp);
		return -1;
	}

	if (s == 0) {
		return -1;
	}

	*s = fp_stat.st_size;

	return 0;

}

int main(int argc, char ** argv)
{
//	size_t fsize;
	int i;

	if (argc < 2) {
		printf("Usage: \n");
		return 0;
	}

	for (i = 0; i < argc; i++) {
		t_input_arg *argsptr;
		char *argvptr;
		argsptr = input_args;
		argvptr = *(argv + i);

		while (argsptr->arg != 0) {
			char *argvalueptr;
			if (strncmp(argsptr->arg, argvptr, strlen(argsptr->arg)) == 0) {
				printf("%s\n", argvptr);
				// Разделим аргемент на составляющие
				argvalueptr = argvptr + strlen(argsptr->arg);
				printf("value: %s\n", argvalueptr);
			}

			argsptr++;
		}

	}

#	if 0
	if (get_file_size(*(argv + 1), &fsize) != 0) {
		printf("Get file size failed\n");
		return -1;

	}

	printf("File size: %zd\n", fsize);
#	endif
	return 0;
}

