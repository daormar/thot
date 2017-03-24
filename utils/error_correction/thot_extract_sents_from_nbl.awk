# *- awk -*

BEGIN{}
{
  if(FNR>1)
  {
    spsymfound=0
    for(i=1;i<=NF;++i)
    {
      if($i=="|||") ++spsymfound
      if(spsymfound==2) break;
    }
    ++i
    for(;i<=NF;++i)
    {
      printf"%s",$i
      if(i!=NF) printf" "
    }
    printf"\n"
  }
}
END{}
