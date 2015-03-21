package test;

import static org.junit.Assert.*;

import java.io.IOException;

import org.junit.*;
import driver.*;

public class TestingFile {

	Directory A;
	Directory B;
	Directory C;
	Directory A1;
	File A_File;

	@Before
	public void setUp() {
		Directory.root = new Directory();
		A = new Directory("A", Directory.root);
		B = new Directory("B", Directory.root);
		C = new Directory("C", Directory.root);
		A1 = new Directory("A1", A);
		A_File = new File("A_File", A);
		Directory.root.setWorkingDirectory();
	}

	@Test
	public void testCreateFile() {
		File A1_1File = new File("A1_1File", A1);
		assertTrue(A1.getFiles().contains(A1_1File));
		assertEquals(A1_1File.getParent(), A1);
	}

	@Test
	public void testDeleteFileSystem() {
		File A1_1File = new File("A1_1File", A1);
		A1_1File.deleteFileSystem();
		assertFalse(A1.getFiles().contains(A1_1File));
		assertEquals(A1_1File.getParent(), null);
	}

	@Test
	public void testalterContents() {
		A_File.alterContents("New Contents!", true);
		assertEquals(A_File.getContents(), "New Contents!");
		A_File.alterContents("More New Contents!", false);
		assertEquals(A_File.getContents(), "New Contents!More New Contents!");
		A_File.alterContents("More New Contents!", true);
		assertEquals(A_File.getContents(), "More New Contents!");
	}
	
	@Test
	public void testCreateFileFromURL() {
		File URLFile = File.createFileFromURL("http://www.cs.cmu.edu/~spok/grimmtmp/073.txt");
		assertEquals(URLFile.getContents(), "");
		assertEquals(URLFile.getName(), "073.txt");
		assertEquals(URLFile.getParent(), Directory.workingDirectory);
	}
		
}