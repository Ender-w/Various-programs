//Written by a1688639 - a1728590
import java.util.*;
import java.io.*;

final class IntPair{
	public final int car;
	public final int cdr;
	public IntPair(int a, int b){
		car = a;
		cdr = b;
	}
	
	//Override functions to use in hashtable
	@Override
	public int hashCode(){
		final int prime = 31;
		int result = 1;
		result = prime * result + car;
		result = prime * result + cdr;
		return result;
	}
	
	@Override
	public boolean equals(Object obj){
		if(!(obj instanceof IntPair))
			return false;
		if(obj == this)
			return true;
		
		IntPair rhs = (IntPair)obj;
		if(car != rhs.car)
			return false;
		if(cdr != rhs.cdr)
			return false;
		return true;
	}
	
	public IntPair (IntPair copy){
		this.car = copy.car;
		this.cdr = copy.cdr;
	}
}

final class IntTriple{
	public final int car;
	public final int cdr;
	public final int cddr;
	public IntTriple(int a, int b, int c){
		car = a;
		cdr = b;
		cddr = c;
	}
	
	//Override functions to use in hashtable
	@Override
	public int hashCode(){
		final int prime = 31;
		int result = 1;
		result = prime * result + car;
		result = prime * result + cdr;
		result = prime * result + cddr;
		return result;
	}
	
	@Override
	public boolean equals(Object obj){
		if(!(obj instanceof IntTriple))
			return false;
		if(obj == this)
			return true;
		
		IntTriple rhs = (IntTriple)obj;
		if(car != rhs.car)
			return false;
		if(cdr != rhs.cdr)
			return false;
		if(cddr != rhs.cddr)
			return false;		
		return true;
	}
	
	public IntTriple (IntTriple copy){
		this.car = copy.car;
		this.cdr = copy.cdr;
		this.cddr = copy.cddr;
	}
}
/*
final class IntQuad{
	public final int car;
	public final int cdr;
	public final int cddr;
	public final int cdddr;
	public IntQuad(int a, int b, int c, int d){
		car = a;
		cdr = b;
		cddr = c;
		cdddr = d;
	}
	
	//Override functions to use in hashtable
	@Override
	public int hashCode(){
		final int prime = 31;
		int result = 1;
		result = prime * result + car;
		result = prime * result + cdr;
		result = prime * result + cddr;
		result = prime * result + cdddr;
		return result;
	}
	
	@Override
	public boolean equals(Object obj){
		if(!(obj instanceof IntQuad))
			return false;
		if(obj == this)
			return true;
		
		IntQuad rhs = (IntQuad)obj;
		if(car != rhs.car)
			return false;
		if(cdr != rhs.cdr)
			return false;
		if(cddr != rhs.cddr)
			return false;
		if(cdddr != rhs. cdddr)
			return false;
		return true;
	}
	
	public IntQuad (IntQuad copy){
		this.car = copy.car;
		this.cdr = copy.cdr;
		this.cddr = copy.cddr;
		this.cdddr = copy.cdddr;
	}
}
*/
class Randomised{
	public static void main(String args[]){
		//String Filename;
		int supportThreshold = 5;
		float samplePercentage = 0.01f;//Default values for supportthresh/sampleperct
		//if(args.length == 1){
		//	Filename = args[0];
		//}
		if(args.length > 1){
			supportThreshold = Integer.parseInt(args[1]);
		}
		if(args.length > 2){
			samplePercentage = Float.parseFloat(args[2]);
		}
		if(args.length > 3 || args.length == 0){
			System.err.println("Incorrect args, Use: (Req){Filename} (Opt){SupportThreshhold} (Opt){SamplePercentage}");
			System.exit(1);
		}
		
		int effectiveThreshold = (int)((float)supportThreshold * samplePercentage);
		
		//Bit of random seed setup so that both passes use the same sample
		Random generator = new Random();
		long seed = generator.nextLong();
		
		System.out.println("Effective threshold: " + effectiveThreshold);
		
		Hashtable<Integer,Integer> singletonCounts = new Hashtable<Integer,Integer>();//For first pass k=1
		Hashtable<IntPair,Integer> counts = new Hashtable<IntPair,Integer>();//For frequent itemsets k=2
		Hashtable<IntTriple,Integer> tripCounts = new Hashtable<IntTriple,Integer>();//For frequent itemsets k=3
		//Hashtable<IntQuad,Integer> quadCounts = new Hashtable<IntQuad,Integer>();//For frequent itemsets k=4 Not enough mem
		
		//----First Pass----
		generator.setSeed(seed);
		File file = new File(args[0]);
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {
			for(String line; (line = br.readLine()) != null; ) {
				//Dont need to map item names to indexes as our baskets already work with indexes
				if(generator.nextFloat() < samplePercentage){//Skip if not part of sample
					String[] splitLine = line.split("\\s+");
				
					for(int i = 0; i < splitLine.length; i++){
						int value = Integer.parseInt(splitLine[i]);
						if(!singletonCounts.containsKey(value)){
							singletonCounts.put(value, 1);//Not already in table
						}else{//already in table
							singletonCounts.put(value, singletonCounts.get(value) + 1);//Increment count
						}
					}
				}
			}
		}catch (Exception e){
			System.err.println("File Error: " + e.getMessage());
			System.exit(2);
		}
		
		//----Between passes----
		//Iterate through singleton map and remove items with less than support threshhold
		//System.out.println(singletonCounts.size());
		Set set = singletonCounts.entrySet();
		Iterator iter = set.iterator();
		while(iter.hasNext()){
			Map.Entry me = (Map.Entry)iter.next();
			int value = (Integer)me.getValue();
			if(value < effectiveThreshold){
				iter.remove();
			}
		}
		System.out.println("Frequent singletons: " + singletonCounts.size());
		
		int test = 0;
		//----Second Pass----
		generator.setSeed(seed);
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {//First Pass
			for(String line; (line = br.readLine()) != null; ) {
				if(generator.nextFloat() < samplePercentage){//Skip if not part of sample
					//Create int array for basket
					String[] splitLine = line.split("\\s+");
					int[] intArray = new int[splitLine.length];
					for(int i = 0; i < splitLine.length; i++){
						intArray[i] = Integer.parseInt(splitLine[i]);
					}
					//generate permutations for k=2,3,4
					Arrays.sort(intArray);
					for(int i = 0; i < intArray.length; i++){
						for(int x = i+1; x < intArray.length; x++){
							if(singletonCounts.containsKey(intArray[i]) && singletonCounts.containsKey(intArray[x])){
								IntPair pair = new IntPair(intArray[i],intArray[x]);
								if(!counts.containsKey(pair)){
									counts.put(pair, 1);
								}else{
									counts.put(pair, counts.get(pair) + 1);
								}
							}
							
							for(int z = x+1; z < intArray.length; z++){
								if(singletonCounts.containsKey(intArray[i]) && singletonCounts.containsKey(intArray[x])
								 && singletonCounts.containsKey(intArray[z])){
								 	IntTriple trip = new IntTriple(intArray[i],intArray[x],intArray[z]);
								 	if(!tripCounts.containsKey(trip)){
								 		tripCounts.put(trip, 1);
								 	}else{
								 		tripCounts.put(trip, tripCounts.get(trip) + 1);
								 	}
								 	test++;
								 }/*
								 for(int y = z+1; y < intArray.length; y++){
								 	if(singletonCounts.containsKey(intArray[i]) && singletonCounts.containsKey(intArray[x])
								 	&& singletonCounts.containsKey(intArray[z]) && singletonCounts.containsKey(intArray[y])){
								 		IntQuad quad = new IntQuad(intArray[i],intArray[x],intArray[z],intArray[y]);
								 		if(!quadCounts.containsKey(quad)){
								 			quadCounts.put(quad,1);
								 		}else{
								 			quadCounts.put(quad, quadCounts.get(quad) + 1);
								 		}
								 	}
								}*/

							}
						}
					}
				}
			}
		}catch (Exception e){
			System.err.println("File Error: " + e.getMessage());
			System.exit(3);
		}
		
		//----Analyse results----
		set = counts.entrySet();
		iter = set.iterator();
		while(iter.hasNext()){
			Map.Entry me = (Map.Entry)iter.next();
			int value = (Integer)me.getValue();
			if(value < effectiveThreshold){
				iter.remove();
			}else{
				IntPair key = new IntPair((IntPair)me.getKey());
				System.out.println("{" + key.car + "," + key.cdr + "} : " + value);
			}
		}
		set = tripCounts.entrySet();
		iter = set.iterator();
		while(iter.hasNext()){
			Map.Entry me = (Map.Entry)iter.next();
			int value = (Integer)me.getValue();
			if(value < effectiveThreshold){
				iter.remove();
			}else{
				IntTriple key = new IntTriple((IntTriple)me.getKey());
				System.out.println("{" + key.car + "," + key.cdr + "," + key.cddr + "} : " + value);
			}
		}
		System.out.println("-------------------");
		System.out.println("Threshold: " + supportThreshold);
		System.out.println("Permutations: " + test);
		System.out.println("Frequent Doubles: " + counts.size());
		System.out.println("Frequent Triples: " + tripCounts.size());
	}
}
