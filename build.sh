SRC="./src"
OUT="./out"

gcc $SRC/shell_sched.c $SRC/utils.c -o $OUT/shell_sched
gcc $SRC/user_scheduler.c $SRC/list_scheduler.c $SRC/exit_scheduler.c $SRC/utils.c -o $OUT/user_scheduler