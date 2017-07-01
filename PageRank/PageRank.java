//Written by a1688639 - a1728590
import java.util.*;
import java.io.*;

class matEntry {
	//int node;
	public int outDegree;
	public double newPageRank;
	public double oldPageRank;
	public ArrayList<Integer> destinations;
	public ArrayList<Integer> sources;
	//875713
	public matEntry(){
		outDegree = 0;
		newPageRank = 1f/875713f;//the 4 is the number of nodes. much easier to hardcode this.
		oldPageRank = 0;
		destinations = new ArrayList<Integer>();
		sources = new ArrayList<Integer>();
	}
}

class PageRank{
	public static void main(String args[]){
		HashMap<Integer, matEntry> hm = new HashMap<Integer, matEntry>(875713);
				
		System.err.println("beginning input parsing");
		
		File file = new File("web-Google.txt");
		try(BufferedReader br = new BufferedReader(new FileReader(file))) {
			for(String line; (line = br.readLine()) != null; ) {
				if(line.charAt(0) != '#'){
					
					String[] parts = line.split("\\t");
					int source = Integer.parseInt(parts[0]);
					int dest = Integer.parseInt(parts[1]);
					if(hm.get(source) == null){//creating new hash map entries for nodes
						hm.put(source, new matEntry());
					}
					if(hm.get(dest) == null){
						hm.put(dest, new matEntry());
					}
					
					hm.get(source).outDegree++;
					hm.get(source).destinations.add(dest);
					
					hm.get(dest).sources.add(source);
				}
			}
		}catch (Exception e){
			System.err.println("File Error: " + e.getMessage());
		}
    	
    	
    	System.err.println("Finished input parsing");
    	System.err.println("Beginning transitions");


    	double betaValue = 0.8;
    	boolean useOldPageRank = false;
    	int nodeCount = hm.size();
		//variable setup
		
    	Set set = hm.entrySet();
    	for(int i = 0; i < 75; i++){//50 iterations
    		Iterator iter = set.iterator();
    		while(iter.hasNext()){
    			Map.Entry me = (Map.Entry)iter.next();
    			matEntry ent = (matEntry)me.getValue();
    			double sum = 0;
    			for(int z = 0; z < ent.sources.size(); z++){//perform vector multiplication
    				double matrixValue = ((1f/(hm.get(ent.sources.get(z)).outDegree)) * betaValue);
					//This value is adjusted accordingly for taxation
    				if(useOldPageRank){
    				
    					sum += hm.get(ent.sources.get(z)).oldPageRank * matrixValue;
    				
    				}else{
    					
    					sum += hm.get(ent.sources.get(z)).newPageRank * matrixValue;
    					
    				}
    			}
    			
    			if(useOldPageRank){
    				ent.newPageRank = sum + (1f-betaValue)/(double)nodeCount;//addition is to take taxation into account
    			}else{
    				ent.oldPageRank = sum + (1f-betaValue)/(double)nodeCount;
    			}
    		}
    		
    		useOldPageRank = !useOldPageRank;//Need to alternate page rank value so that the old page ranks are not overwritten before we are done using them.
    		System.err.println("Finished transition " + (i+1));
    	}
    	
		System.err.println("Finished all transitions");
		System.err.println("Beginning output");
		
		//a few variables and setup for calculating the top 10 pageranks
		boolean topTen = false;
		double[] topTenList = new double[10];
		int[] topTenListIndex = new int[10];
		for(int i = 0; i < 10; i++){
			topTenList[i] = 0f;
			topTenListIndex[i] = 0;
		}
		
		double smallestLargest = 0;//the smallest value of the largest 10 values
		
    	Set set2 = hm.entrySet();
		Iterator iter2 = set2.iterator();
    	while(iter2.hasNext()){
			if(!topTen){
				Map.Entry me = (Map.Entry)iter2.next();
				matEntry ent = (matEntry)me.getValue();
				System.out.print(me.getKey() + ": ");
				if(useOldPageRank){
					System.out.println(ent.oldPageRank);
				}else{
					System.out.println(ent.newPageRank);
				}
			}else{
				Map.Entry me = (Map.Entry)iter2.next();
				matEntry ent = (matEntry)me.getValue();
				double pageRank;
				if(useOldPageRank){
					pageRank = ent.oldPageRank;
				}else{
					pageRank = ent.newPageRank;
				}
								
				if(pageRank > smallestLargest){
					for(int x = 0; x<10; x++){
						if(pageRank > topTenList[x+1]){
							//do nothing/go to next iteration unless its the last iteration
							if(x+1 == 9){
								topTenList[9] = pageRank;
								topTenListIndex[9] = (int)me.getKey();
								break;
							}
						}else{
							topTenList[x] = pageRank;
							topTenListIndex[x] = (int)me.getKey();
							if(x == 0){
								smallestLargest = pageRank;
							}
							break;
						}
					}
				}
			}
    	}
    	
    	if(topTen){
			System.out.println("-Highest pageranks-(Decsending order)");
			for(int i = 9; i >= 0; i--){
				System.out.print(topTenListIndex[i] + ": ");
				System.out.println(topTenList[i]);
			}
    	}
    	System.err.println("Finished output - Program done");

    }  
}  
