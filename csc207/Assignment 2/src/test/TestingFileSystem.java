package test;

import static org.junit.Assert.*;
import org.junit.*;
import driver.*;

public class TestingFileSystem {

	Directory A;
	Directory B;
	Directory C;
	Directory A1;

	@Before
	public void setUp() {
		Directory.root = new Directory();
		A = new Directory("A", Directory.root);
		B = new Directory("B", Directory.root);
		C = new Directory("C", Directory.root);
		A1 = new Directory("A1", A);

	}

	@Test
	public void testCreateFileSystem() {
		FileSystem A11 = new FileSystem("A11", A1);
		assertTrue(A11.getName() == "A11");
		assertEquals(A11.getParent(), A1);
	}

	@Test
	public void testDeleteFileSystem() {
		FileSystem A11 = new FileSystem("A11", A1);
		A11.deleteFileSystem();
		assertEquals(A11.getParent(), null);
	}

}