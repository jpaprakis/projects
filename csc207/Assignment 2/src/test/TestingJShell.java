package test;

import static org.junit.Assert.*;

import org.junit.*;

import driver.*;

public class TestingJShell {

	@Before
	public void setUp() {
		Directory.root = new Directory();
	}

	@Test
	public void testSetPrompt() {
		JShell.setPrompt("/A");
		assertEquals(JShell.getPrompt(), "/A");
	}

}
