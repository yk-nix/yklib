define(msg, 
	this is a good news:
	It's a nice day! (
	)
)dnl
1msg-

define(N, 100)dnl
define(M, `N')dnl
`N': N
`M': M
undefine(`N')dnl
ifdef(`N', `define(L, `N')', `define(L, N)')dnl
define(N, 200)dnl
`N': N
`M': M
`L': L

define(sum, $1+$2)dnl
sum(shift(shift(1,2,3,4)))
define(number, `incr(10)')dnl
number      decr(number)       eval(10*number)
`eval(relational-expression)': eval(number == decr(incr(number)))
`eval(relational-expression)': eval(1==2)
`divnum': divnum
changequote(^)dnl
^syscmd(cmd)': syscmd(^echo -n `date "+%Y-%m-%d %H:%M:%S.%N"`; echo -n "    "; echo "hello, yoka"')
changequote(`)dnl
`syscmd(cmd)': syscmd(`echo Its a nice day!')
maketemp()dnl

define(compare, `ifelse($1, $2, yes, no)')dnl
`compare(a,b,c,...)': compare(100, 100)
`len(str)': len(`hello, yoka')
`substr(str, start, len)': substr(`Hello,yoka', 6)
`substr(str, start, len)': stubstr(`Hello,yoka', 6, 3)
`index(str, substr)': index(`hello, yoka', `yoka')
`translit(str, charset1, charset2)': translit(`hello, yoka', `abcdefghijklmnopqrstuvwxyz', `ABCDEFGHIJKLMNOPQRSTUVWXYZ')
`translit(str, charset1, charset2)': translit(`hello, yoka', ` yo')
#changecom(?)dnl
#?dumpdef(`msg')dnl
traceon()
