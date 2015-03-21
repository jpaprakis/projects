package driver;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;

public class Author {

	private StringBuilder name;
	private StringBuilder numCitations;
	private StringBuilder numi10Citations;
	private List<String> pubs;
	private int numPaperCitations;
	private int numCoAuthors;
	private List<String> coAuthors;
	private Output output;

	private static List<String> fullCoAuthorList = new ArrayList<String>();
	
	private final String regExName = "<span id=\"cit-name-display\" "
            + "class=\"cit-in-place-nohover\">(.*?)</span>";
	private final String regExNumCitations = ">Citations</a></td><td class=\""
			+ "cit-borderleft cit-data\">(\\d+)</td>";
	
	private final String regExNumi10Citations = "<td class=\"cit-borderleft "
			+ "cit-data\">(\\d+)</td></tr>((</tbody>)?)</table>";
	
	private final String regExPubs = "class=\"cit-dark-large-link\">(.*?)</a>"
			+ "<br>";
	
	private final String regExPaperCitations = ">(\\d+)</a></td><td id=\"col-"
			+ "asterisk\"></td><td id=\"col-year\">";
	
	private final String regExNameCoAuthor = "\" title=\"(.*?)\">(.*?)</a><br>"
			+ "<(a|d)";

	
	/**
	 *  Constructor for Author - sets all the instance variables of the Author
	 *  to the StringBuilder/ArrayList desired, by calling either simpleExtract
	 *  or listExtract from the Extractor class on their respective regexes 
	 * 
	 * @param googleScholarURL
	 *            the URL of the current Author
	 */
	public Author(String googleScholarURL) {
		try {
		//Constructor sets the current parsedURL to be that of the current 
		//Author by calling extractor
		Extractor authorExtractor = new Extractor(googleScholarURL);
		this.name = authorExtractor.simpleExtract(regExName);
		this.numCitations = authorExtractor.simpleExtract(regExNumCitations);
		this.numi10Citations = authorExtractor.simpleExtract(
				regExNumi10Citations);
		this.pubs = authorExtractor.listExtract(regExPubs, 3, 1);
		this.numPaperCitations = countCitations(authorExtractor.listExtract(
				regExPaperCitations, 5, 1));
		this.coAuthors = authorExtractor.listExtract(regExNameCoAuthor, -1, 2);
		this.numCoAuthors = this.coAuthors.size();
		fullCoAuthorList.addAll(this.coAuthors);
		//Create a new instance of Output for this Author and store it in this
		//Author's output instance variable
      	this.output = new Output(this);
		}
		catch (Exception e) {
			System.out.println("malformed URL or cannot open connection to "
			          + "given URL");
		}
	}
	
	/**
	 *  Iterates through the given list of whole numbers stored as Strings,
	 *  adds them together and returns them
	 * 
	 * @param citationSet
	 *            the list of whole numbers stored as Strings
	 *            
	 *  @return the sum of all citations in the list
	 */	
	public static int countCitations(List<String> citationSet) {
		Iterator<String> iterator = citationSet.iterator();
		int sumOfCitations = 0;
		while (iterator.hasNext()) {
			int toInt = Integer.parseInt((String)(iterator.next()));
			sumOfCitations += toInt;
		}
		return sumOfCitations;
	}
	
	/**
	 *  Gets the Author name
	 * 
	 * @return the StringBuilder stored in the Author's instance variable name
	 *            
	 */	
	public StringBuilder getName() {
		return name;
	}

	/**
	 *  Gets the Author's number of citations
	 * 
	 * @return the StringBuilder stored in the Author's instance variable 
	 * numCitations
	 *            
	 */	
	public StringBuilder getNumCitations() {
		return numCitations;
	}
	
	/**
	 *  Gets the Author's number of i10-index citations after 2009 
	 * 
	 * @return the StringBuilder stored in the Author's instance variable 
	 * numi10Citations
	 *            
	 */	
	public StringBuilder getNumi10Citations() {
		return numi10Citations;
	}

	/**
	 *  Gets the Author's publications
	 * 
	 * @return the arrayList of 3 publications stored in the Author's instance 
	 * variable pubs
	 *            
	 */	
	public List<String> getPubs() {
		return pubs;
	}

	/**
	 *  Gets the Author's number of paper citations
	 * 
	 * @return the int stored in the Author's instance variable 
	 * numPaperCitations
	 *            
	 */	
	public int getNumPaperCitations() {
		return numPaperCitations;
	}

	/**
	 *  Gets the Author's number of coAuthors
	 * 
	 * @return the int stored in the Author's instance variable numCoAuthors
	 *            
	 */	
	public int getNumCoAuthors() {
		return numCoAuthors;
	}
	
	/**
	 *  Gets the full list of all coAuthors
	 * 
	 * @return the static list stored as fullCoAuthorList
	 *            
	 */	
	public static List<String> getFullCoAuthorList() {
		return fullCoAuthorList;
	}
	
	/**
	 *  Returns the contents of the output instance object for the Author 
	 *  (Only used for testing)
	 * 
	 * @return
	 * 				Author Contents of the instance of output, of type 
	 * 				StringBuilder
	 *            
	 */	
	public StringBuilder getOutput() {
		return output.getAuthorContents();
	}
	
	/**
	 *  Adds an element to the list of all coAuthors (only used for testing)
	 * 
	 * @param	itemToAdd
	 * 				The (string) element to add to the list 
	 *            
	 */	
	
	public static void setFullCoAuthorList(String itemToAdd) {
		fullCoAuthorList.add(itemToAdd);
	}
	
	/**
	 *  Removes duplicates from fullCoAuthorList by storing it as a HashSet, 
	 *  then returning it back to the ArrayList. Then calls the sort method
	 *  which sorts the list alphabetically
	 * 
	 */	
	public static void removeDuplicatesAndSort() {
		HashSet<String> tempCoAuthorList = new HashSet<String>();
		tempCoAuthorList.addAll(fullCoAuthorList);
		fullCoAuthorList.clear();
		fullCoAuthorList.addAll(tempCoAuthorList);
		java.util.Collections.sort(fullCoAuthorList);
	}

}


