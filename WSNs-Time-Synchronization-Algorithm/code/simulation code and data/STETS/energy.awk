BEGIN{
n=241;
}
{
nt=$1;
e=$7;
id=$5;
if(nt=="N")
	arr[id]=e;
}
END {
for(j=0;j<n;j++){
	sum+=100-arr[j];
}	
printf("%.9f\n", sum/n);
}
