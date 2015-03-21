package test;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.List;

import org.junit.BeforeClass;
import org.junit.Test;

import driver.Author;

public class TestAuthor {

	String testURL = "sample1.html";

	
	@Test
	public void testAuthorConstructor() {
		String testURL = "sample1.html";
		List<String> pubList = new ArrayList();
		pubList.add("Bioclipse: an open source workbench for chemo"
				+ "-and bioinformatics");
		pubList.add("The LCB data warehouse");
		pubList.add("XMPP for cloud computing in bioinformatics supporting "
								+ "discovery and invocation of asynchronous "
								+ "web services");

		Author testAuthor = new Author(testURL);
		assertEquals("Ola Spjuth", testAuthor.getName().toString());
		assertEquals("437", testAuthor.getNumCitations().toString());
		assertEquals("12", testAuthor.getNumi10Citations().toString());
		assertEquals(pubList, testAuthor.getPubs());
		assertEquals(239, testAuthor.getNumPaperCitations());
		assertEquals(15, testAuthor.getNumCoAuthors());
	}

	@Test
	public void testCountCitations() {
		List<String> testCitationSet = new ArrayList();
		testCitationSet.add("1");
		testCitationSet.add("1");
		testCitationSet.add("5");
		testCitationSet.add("5");
		testCitationSet.add("10");		
		assertEquals(22, Author.countCitations(testCitationSet));
	}
	
	@BeforeClass
	public static void setUpFullCoAuthorList() {
		Author.setFullCoAuthorList("Abigail Sellen");
		Author.setFullCoAuthorList("Sam Adams");
		Author.setFullCoAuthorList("E.D. Mynatt");
		Author.setFullCoAuthorList("Abigail Sellen");
		Author.setFullCoAuthorList("Rajarshi Guha");
		Author.setFullCoAuthorList("Sam Adams");
	}
	
	@Test
	public void testRemoveDuplicatesAndSort() {
		List<String> testFullCitationSet = new ArrayList();
		testFullCitationSet.add("Abigail Sellen");
		testFullCitationSet.add("E.D. Mynatt");
		testFullCitationSet.add("Rajarshi Guha");
		testFullCitationSet.add("Sam Adams");
		Author.removeDuplicatesAndSort();
		assertEquals(Author.getFullCoAuthorList(), testFullCitationSet);
	}
	
}
