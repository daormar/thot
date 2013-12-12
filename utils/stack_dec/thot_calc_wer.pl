# Author: Daniel Ortiz Mart\'inez
# *- perl -*

# Given reference and system sentence files, calculates the WER, PER,
# and SER measures.

# \textbf{Categ}: Error_measuring

$numPar=@ARGV;
if($numPar!=2 && $numPar!=3) 
{
    print "Usage: calc_wer <ref_file> <out_file> <-v|-v1>\n";
}
else
{
 open(REF_FILE,$ARGV[0]) or die "Error while opening file with reference sentences";	
 open(OUT_FILE,$ARGV[1]) or die "Error while opening file with system sentences";
 	
 $acum{"WER"}=0;
 $acum{"PER"}=0;
 $acum{"SER"}=0;
 $acum{"InsWer"}=0;
 $acum{"SubWer"}=0;
 $acum{"DelWer"}=0;
 $acum{"InsPer"}=0;
 $acum{"SubPer"}=0;
 $acum{"DelPer"}=0;
 
 $acumWords=0;
 $numSent=0;
 if($ARGV[2] eq "-v")
 {
     $verbose=1;
 }
 else
 {
     if($ARGV[2] eq "-v1")
     {
         $verbose=2;
     }
     else
     {
         $verbose=0;
     }
 }
 
 while(<REF_FILE>)
 {
     $numSent=$numSent+1;
     $refSent=$_;  
	   	  
     $outSent=<OUT_FILE>;
     chomp($outSent);
     chomp($refSent);

     $refSent=~ s/^\s+//; # remove leading blanks
     $refSent=~ s/\s+$//; # remove trailing blanks
     $outSent=~ s/^\s+//; # remove leading blanks
     $outSent=~ s/\s+$//; # remove trailing blanks

     @refSentArray=split(/ /,$refSent);	
		  
     if($verbose==2)
     {
         printf("Sentence pair #%d\n",$numSent);
             printf(" ref: %s\n",$refSent);
         printf(" out: %s\n",$outSent);
     }
     $numRefWords=@refSentArray;	
     $acumWords=$acumWords+$numRefWords;	  
     $wer=edit_dist($refSent,$outSent,1,1,1);
     $per=calculatePER($refSent,$outSent,1,1,1)*$numRefWords;
     $ser=diff($outSent,$refSent);
     
     $acum{"WER"}=$acum{"WER"}+$wer;
     $acum{"PER"}=$acum{"PER"}+$per; 
     $acum{"SER"}=$acum{"SER"}+$ser;
     
     if($verbose==1)
     {
         printf("%g %s ||| %s\n",$wer/$numRefWords,$refSent,$outSent);
     }
 }
 if($verbose!=1)
 {
     printf("WER = %.2f\nPER = %.2f\nSER = %.2f\n",($acum{"WER"}/$acumWords)*100,($acum{"PER"}/$acumWords)*100,($acum{"SER"}/$numSent)*100); 
 }
 if($verbose==2)
 {
     printf("AcumWer ops: [ Ins Sub Del ] [ %d %d %d ]\n",$acum{"InsWer"},$acum{"SubWer"},$acum{"DelWer"});
     printf("AcumPer ops: [ Ins Sub Del ] [ %d %d %d ]\n",$acum{"InsPer"},$acum{"SubPer"},$acum{"DelPer"});
 }
 close(OUT_FILE);
 close(REF_FILE);
}

#---------------------------------------------	
sub edit_dist # Parametros: (x,y,iw,sw,dw): cadenas origen y destino y costes de ins, sub y borr.
             # Perl almacena los parametros en el array $_
{
 my $x=$_[0]; 
 my $y=$_[1];
 my $iw=$_[2]; 
 my $sw=$_[3]; 
 my $dw=$_[4];	
 my @xArray=split(/ +/,$x); 
 my @yArray=split(/ +/,$y);
 my $tx = @xArray;
 my $ty = @yArray;
 $InsWer=0;
 $SubWer=0;
 $DelWer=0;
 
 #printf("->%s , %s |%d,%d,%d,%d,%d\n",$x,$y,$iw,$sw,$dw,$tx,$ty);	
 $D[0][0] = 0;
 for (my $i=1; $i<=$tx; $i++) {$D[$i][0] = $D[$i-1][0] + $dw;}
 for (my $j=1; $j<=$ty; $j++) {$D[0][$j] = $D[0][$j-1] + $iw;}

 for (my $i=1; $i<=$tx; $i++)
     {for (my $j=1; $j<=$ty; $j++) 
          {
           if($xArray[$i-1] eq $yArray[$j-1]) {$sum=0;}
           else {$sum=$sw;}
           $min = $D[$i-1][$j-1] + $sum;
           
           if ($D[$i-1][$j]+$dw < $min) {$min = $D[$i-1][$j]+$dw; }
           if ($D[$i][$j-1]+$iw < $min) {$min = $D[$i][$j-1]+$iw; }
           $D[$i][$j] = $min;
		  }	   
     }
	 
 $i=$tx;
 $j=$ty;
 while($i>0 || $j>0)
 {
  if($i>0 && $j>0)
  {if($D[$i-1][$j-1]<= $D[$i-1][$j] && $D[$i-1][$j-1]<=  $D[$i][$j-1])
     {if($D[$i-1][$j-1]<$D[$i][$j]){++$SubWer;}
      --$i;
      --$j; 
     }
   else
   {if($i>0 && $D[$i-1][$j]<= $D[$i][$j-1])
      {--$i;
	   ++$DelWer;
      }
    else
    {--$j;
     ++$InsWer;
    }
   }
  }
  else{if($i>0){--$i;
	            ++$DelWer;
               }
	   else{--$j;
            ++$InsWer;
           }
      }
 }
 
 $acum{"InsWer"}+=$InsWer;
 $acum{"SubWer"}+=$SubWer;
 $acum{"DelWer"}+=$DelWer;
 if($verbose==2){printf(" WER ops: [ Ins Sub Del ][ %d %d %d ]\n",$InsWer,$SubWer,$DelWer);
                }
 return $D[$tx][$ty];
}				
#---------------------------------------------	

sub calculatePER # Parametros: (Ref,Hyp,iw,sw,dw): cadenas origen y destino y costes de ins, sub y borr.
                 # Perl almacena los parametros en el array $_
{
 my $Ref=$_[0]; 
 my $Hyp=$_[1];
 my $iw=$_[2]; 
 my $sw=$_[3]; 
 my $dw=$_[4];	
 my @refArray=split(/ +/,$Ref); 
 my @hypArray=split(/ +/,$Hyp);
 my $tref = @refArray;
 my $thyp = @hypArray;
  
 for(my $i=0;$i<$tref;$i++) 
    {
     $RefMark[$i]=0;
     $RefMark2[$i]=0;  # to avoid double count of multiple occurrence
    }
 for(my $i=0;$i<$thyp;$i++) 
    {$HypMark[$i]=0;}
	
 #Calculate Error

 # Mark words found in hypothesis and reference sentence
 for(my $i=0;$i<$thyp;$i++)
    {for(my $j=0;$j<$tref;$j++) 
	    {
		 if (($hypArray[$i] eq $refArray[$j] ) && ( $RefMark2[$j] == 0)) 
		    {
	         $HypMark[$i]=1;
	         $RefMark[$j]=1;
	         $RefMark2[$j]=1;  # For multiple word occurence
	         last;
	        }
        }
    }

 #Count Errors
 $hyperror=0;
 for(my $i=0;$i<$thyp;$i++) 
    {if($HypMark[$i]==0){$hyperror++;}
    }
 $referror=0;
 for(my $i=0;$i<$tref;$i++) 
    {if($RefMark[$i]==0){$referror++;}
    }
 
 # Make distinction between length of reference and hypothesis
 if($tref > $thyp) 
   {$Ins=0;
    $Sub=$hyperror;
    $Del=$referror - $hyperror;
   } 
 else{
      $Del=0;
      $Sub=$referror;
      $Ins=$hyperror - $referror;
     }

 $acum{"InsPer"}+=$Ins;
 $acum{"SubPer"}+=$Sub;
 $acum{"DelPer"}+=$Del;
 if($verbose==2){printf(" PER ops: [ Ins Sub Del ][ %d %d %d ]\n",$Ins,$Sub,$Del);
                }
 return ($Ins*$iw + $Del*$dw + $Sub*$sw)/$tref;	
}				
#---------------------------------------------	
sub diff
{ 
 my $x=$_[0]; 
 my $y=$_[1];
 my @xArray=split(/ +/,$x);
 my @yArray=split(/ +/,$y);
 my $tx = @xArray;
 my $ty = @yArray;
 
 if($tx ne $ty) {return 1;}
 for (my $i=0; $i<$tx; $i++) {if($xArray[$i] ne $yArray[$i])
                                   {return 1;}
							  }
 return 0;
}

#---------------------------------------------
