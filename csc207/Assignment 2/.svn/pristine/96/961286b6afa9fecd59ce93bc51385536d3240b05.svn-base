package test;

import static org.junit.Assert.*;

import org.junit.*;

import driver.*;

public class TestingAllCommands {

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
	}

	@Test
	public void testEchoFile() {
		B.setWorkingDirectory();
		// If the file doesn't already exist
		AllCommands.echoFile("Hello", "BFile", true);
		assertTrue(B.fileSystemInDirectory("BFile"));
		assertEquals(((File) B.fileSystemNameToObject("BFile")).getContents(),
				"Hello");
		assertEquals(((File) B.fileSystemNameToObject("BFile")).getParent(), B);
		// If the file already exists
		A.setWorkingDirectory();
		AllCommands.echoFile("Hello I'm A_File", "A_File", true);
		assertTrue(A.fileSystemInDirectory("A_File"));
		assertEquals(((File) A.fileSystemNameToObject("A_File")).getContents(),
				"Hello I'm A_File");
		assertEquals(((File) A.fileSystemNameToObject("A_File")).getParent(), A);
		A.fileSystemNameToObject("A_File").deleteFileSystem();
		// ensure there are no duplicate files
		// once the file is deleted it shouldn't exist anymore
		assertFalse(A.fileSystemInDirectory("A_File"));
	}

	@Test
	public void testChangeWorkingDirectory() {
		// Changing directories from the root to A1
		Directory.root.setWorkingDirectory();
		AllCommands.changeWorkingDirectory(A1);
		assertEquals(Directory.workingDirectory, A1);
		assertEquals(JShell.getPrompt(), "/A/A1");
		// Changing directories from A1 to the root
		AllCommands.changeWorkingDirectory(Directory.root);
		assertEquals(Directory.workingDirectory, Directory.root);
		assertEquals(JShell.getPrompt(), "");
	}

}
