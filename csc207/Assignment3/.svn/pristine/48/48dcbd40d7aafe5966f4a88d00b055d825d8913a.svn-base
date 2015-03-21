package test;

import static org.junit.Assert.*;

import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.Test;

import driver.Extractor;

public class TestExtractor {

	Extractor testExtractor;
	
	@Before
	public void setUpHTML() {
		try {
			testExtractor = new Extractor("sample1.html");
		} catch (Exception e) {
			System.out.println("bad URL");
		}
	}
	
	@Test
	public void testSimpleExtract() {
		String regExNumi10Citations = "<td class=\"cit-borderleft "
				+ "cit-data\">(\\d+)</td></tr>((</tbody>)?)</table>";
		assertEquals("12",testExtractor.simpleExtract(regExNumi10Citations)
				.toString());
	}
	
	@Test
	public void testListExtract() {
		String regExPubs = "class=\"cit-dark-large-link\">(.*?)</a>"
				+ "<br>";
		List<String> pubList = new ArrayList();
		pubList.add("Bioclipse: an open source workbench for chemo"
				+ "-and bioinformatics");
		pubList.add("The LCB data warehouse");
		pubList.add("XMPP for cloud computing in bioinformatics supporting "
								+ "discovery and invocation of asynchronous "
								+ "web services");
		assertEquals(pubList,testExtractor.listExtract(regExPubs, 3, 1));
	}
	

}
