BEGIN{
send=0;
recv=0;
}
{
if($1=="s")
	send++;
}
END {
printf("%d\n", send);
}
