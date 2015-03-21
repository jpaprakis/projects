package test;

import static org.junit.Assert.*;

import org.junit.Test;

import driver.MyParser;

public class TestMyParser {

	@Test
	public void testGetHTML() {
		MyParser parser = new MyParser();
		String htmlToTest;
		try {
			htmlToTest = parser.getHTML("sample1.html");
			assertTrue(htmlToTest.startsWith("<!DOCTYPE"));
			assertTrue(htmlToTest.endsWith("</html>"));
		} catch (Exception e) {
			System.out.println("Bad URL");
		}
		
	}

}
