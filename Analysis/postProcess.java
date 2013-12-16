import java.util.*;
import java.io.*;

public class postProcess {
	public static void main(String[] args)
	{
		String file = "results"+ args[0]+".txt";
		File f = new File(file);
		PrintWriter w = null;
		try{
			w = new PrintWriter(new FileWriter("post"+args[0]+".txt", true));
		}catch(Exception e){;}

		String[] ratio = {"0.111111","0.125000","0.142857","0.166667","0.200000","0.222222","0.250000","0.285714","0.333333",
				"0.375000","0.400000",",0.428571","0.444444","0.500000","0.555556","0.571429","0.600000","0.625000","0.666667","0.714286",
				"0.750000","0.777778","0.800000","0.833333","0.857143","0.875000","0.888889","1.000000","1.111111","1.125000","1.142857",
				"1.166667","1.200000","1.250000","1.285714","1.333333","1.400000","1.428571","1.500000","1.600000","1.666667","1.750000",
				"2.000000","2.250000","2.333333","2.500000","2.666667","3.000000","3.333333","3.500000","4.000000",
				"4.500000","5.000000","6.000000","7.000000","8.000000","9.000000","10.000000"};

		for(int i = 0; i < ratio.length; i++)
		{
			Scanner in;
			try{
				System.out.println("here "+i);
				in = new Scanner(f);
				
				in.nextLine();  //discard first line

				double avg = 0;
				double count = 0;

				while(in.hasNextLine())
				{
					String str = in.nextLine();
					//System.out.println(str);
					Scanner line = new Scanner(str);

					//discard first 5
					for(int j = 0; j < 5; j++)
						line.next();

					String r = line.next();
					//System.out.println(r+" ");
					if(!ratio[i].equals(r))
						continue;

					//discard two more
					for(int j = 0; j < 2; j++)
						line.next();

					String s = line.next();
					//System.out.println("adding: "+s);
					avg += Double.parseDouble(s);
					count++;
				}
				if(count > 0)
					w.println(ratio[i] + " "+avg/count);
				else
					w.println(ratio[i] + " "+0);
				//w.flush();
			}catch(Exception e){;}
		}
		
		w.close();
	}

}
