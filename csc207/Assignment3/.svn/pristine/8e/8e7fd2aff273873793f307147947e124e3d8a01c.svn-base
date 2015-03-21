package driver;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Extractor {
	
	private String parsedURL;
	
	/**
	 *  Parses the googleScholarURL and saves the HTML contents to parsedURL
	 * 
	 * @param googleScholarURL
	 *            the URL of the webpage to parse HTML from
	 */
	public Extractor (String googleScholarURL) throws Exception {
		      MyParser googleScholarParser = new MyParser();
		      this.parsedURL = googleScholarParser.getHTML(googleScholarURL);
		      }

	/**
	 *  Extracts the string matching group 1 of the input regex from whatever
	 *  is saved currently as parsedURL and returns it as a StringBuilder 
	 * 
	 * @param regEx
	 *            the regular expression to match
	 *     
	 *@return a StringBuilder of the part of parsedURL that matched the entered
	 *			regex, at group 1
	 */
	public StringBuilder simpleExtract(String regEx) {
		      Pattern patternObject = Pattern.compile(regEx);
		      Matcher matcherObject = patternObject.matcher(this.parsedURL);
		      StringBuilder matchedString = new StringBuilder();
		      while (matcherObject.find()) {
		        matchedString.append(matcherObject.group(1));
		      }
		      return matchedString;
	}
	
	/**
	 *  Extracts the string matching the input regex at the specified groupNum
	 *  from whatever is saved currently as parsedURL and appends each match
	 *  to an ArrayList that it returns. Will stop looking for matches &
	 *  appending once the ArrayList reaches the specified size. If a negative
	 *  number is entered as the desired size, will append all matches 
	 * 
	 * @param regEx
	 *            the regular expression to match
	 * @param setSize
	 * 			   the amount of matches that the arrayList should append
	 * @param groupNum
	 *             the group of the regex to be appended 
	 *             
	 * @return a list of each part of parsedURL that matched the entered
	 *			regex, at groupNum, of size setSize (or all matched if
	 *			setSize < 0)
	 */
	public List<String> listExtract(String regEx, int setSize, int groupNum) {
	      Pattern patternObject = Pattern.compile(regEx);
	      Matcher matcherObject = patternObject.matcher(this.parsedURL);
	      List<String> matchedSet = new ArrayList<String>();
	      int i = 0;
	      //appends to the list while the matcher finds the pattern, AND
	      //counter i is less than the specified set size - if setSize < 0,
	      //then there is no size restriction
	      while (matcherObject.find() && ((i < setSize) || setSize < 0)) {
	        matchedSet.add(matcherObject.group(groupNum));
	        i ++;
	      }
	      return matchedSet;
	}
	
}