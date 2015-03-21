package test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import driver.Author;
import driver.Output;

public class TestOutput {

	Author testAuthor1;
	Author testAuthor2;
	StringBuilder expectedOutput = new StringBuilder();
	private static String nLine = System.getProperty("line.separator");
	
	@Before
	public void setUp() {
		testAuthor1 = new Author("Sample1.html");
		Author.removeDuplicatesAndSort();
		Output.addCoAuthorContents();
		expectedOutput.append("-----------------------------------------------"
					+ "------------------------" + nLine);
		expectedOutput.append("1. Name of Author:" + nLine);
		expectedOutput.append("\tOla Spjuth" + nLine);
		expectedOutput.append("2. Number of All Citations:" + nLine);
		expectedOutput.append("\t437" + nLine);		
		expectedOutput.append("3. Number of i10-index after 2009:" + nLine);
		expectedOutput.append("\t12" + nLine);
		expectedOutput.append("4. Title of the first 3 publications:" + nLine);
		expectedOutput.append("\t1-\tBioclipse: an open source workbench for "
				+ "chemo-and bioinformatics" + nLine);	
		expectedOutput.append("\t2-\tThe LCB data warehouse" + nLine);
		expectedOutput.append("\t3-\tXMPP for cloud computing in "
				+ "bioinformatics supporting discovery and invocation of "
				+ "asynchronous web services" + nLine);		
		expectedOutput.append("5. Total paper citation (first 5 papers):" 
		+ nLine);
		expectedOutput.append("\t" + 239 + nLine);
		expectedOutput.append("6. Total Co-Authors:" + nLine);
		expectedOutput.append("\t" + 15 + nLine);
	}
	
	@Test
	public void testAuthorOutput() {
		assertEquals(expectedOutput.toString(), (testAuthor1.getOutput())
				.toString());
	}
	
	@Test
	public void testAllOutput() {
		expectedOutput.append("---------------------------------------------"
				+ "--------------------------" + nLine);
		expectedOutput.append("7. Co-Author list sorted (Total: 15):" + nLine);
		expectedOutput.append("Adam Ameur"+ nLine);
		expectedOutput.append("Antony John Williams"+ nLine);
		expectedOutput.append("Christoph Steinbeck"+ nLine);
		expectedOutput.append("Egon Willighagen"+ nLine);
		expectedOutput.append("Janna Hastings"+ nLine);
		expectedOutput.append("Jonathan Alvarsson"+ nLine);
		expectedOutput.append("Komorowski Jan"+ nLine);
		expectedOutput.append("Nina Jeliazkova"+ nLine);
		expectedOutput.append("Noel M. O'Boyle"+ nLine);
		expectedOutput.append("Rajarshi Guha"+ nLine);
		expectedOutput.append("Sam Adams"+ nLine);
		expectedOutput.append("Samuel Lampa"+ nLine);
		expectedOutput.append("Sean Ekins"+ nLine);
		expectedOutput.append("Valentin Georgiev"+ nLine);
		expectedOutput.append("gilleain torrance");
		assertEquals(expectedOutput.toString(), (Output.getAllContents())
				.toString());
	}

}
