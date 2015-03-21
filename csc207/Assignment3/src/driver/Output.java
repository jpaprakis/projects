package driver;

import java.util.Iterator;

public class Output {

	private StringBuilder authorContents = new StringBuilder();
	private static StringBuilder allContents = new StringBuilder();
	private static String nLine = System.getProperty("line.separator");
	
	/**
	 *  Creates an instance of output and populates its instance variable
	 *  authorContents for each Author using the Author's instance variables
	 * 
	 * @param currentAuthor
	 *            the current Author instance that output is being populated
	 *            for
	 */
	public Output (Author currentAuthor) {
		//setting up iterator & counter for publications (so they can be
		//easily changed if the user decides to add more than 3)
		Iterator<String> pubIterator = currentAuthor.getPubs().iterator();
		int pubCount = 1;
		
		authorContents.append("-----------------------------------------------"
				+ "------------------------" + nLine);
		authorContents.append("1. Name of Author:" + nLine);
		authorContents.append("\t" + currentAuthor.getName() + nLine);
		authorContents.append("2. Number of All Citations:" + nLine);
		authorContents.append("\t" + currentAuthor.getNumCitations() + nLine);		
		authorContents.append("3. Number of i10-index after 2009:" + nLine);
		authorContents.append("\t" + currentAuthor.getNumi10Citations() 
				+ nLine);
		authorContents.append("4. Title of the first 3 publications:" + nLine);
		while (pubIterator.hasNext()) {
			authorContents.append("\t" + pubCount + "-\t" + pubIterator.next() 
					+ nLine);		
			pubCount++;
		}
		authorContents.append("5. Total paper citation (first 5 papers):" 
		+ nLine);
		authorContents.append("\t" + currentAuthor.getNumPaperCitations() 
				+ nLine);
		authorContents.append("6. Total Co-Authors:" + nLine);
		authorContents.append("\t" + currentAuthor.getNumCoAuthors() + nLine);
		allContents.append(authorContents);
	}
	
	/**
	 *  Returns authorContents
	 * 
	 * @return 
	 *            authorContents, the current StringBuilder that contains
	 *            the author's specific output
	 */
	public StringBuilder getAuthorContents() {
		return authorContents;
	}

	/**
	 *  adds the list containing coAuthorContents to allContents in the correct
	 *  format
	 * 
	 */
	public static void addCoAuthorContents() {
		//Setting up iterator for coAuthor list
		Iterator<String> coAuthorIterator = Author.getFullCoAuthorList()
				.iterator();
		
		allContents.append("---------------------------------------------"
				+ "--------------------------" + nLine);
		allContents.append("7. Co-Author list sorted (Total: " + Author
				.getFullCoAuthorList().size() + "):");
		while (coAuthorIterator.hasNext()) {
			allContents.append(nLine + coAuthorIterator.next());
		}
		
	}
	
	/**
	 *  Returns allContents
	 * 
	 * @return 
	 *            allContents, the static StringBuilder that contains
	 *            all the output contents that have been appended so far
	 */
	public static StringBuilder getAllContents() {
		return allContents;
	}
}
