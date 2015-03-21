package test;

import static org.junit.Assert.*;

import org.junit.*;

import driver.*;

public class TestingFileMovement {

	Directory A;
	Directory B;
	Directory C;
	Directory A1;
	Directory B1;
	Directory C1;
	Directory B11;
	Directory C11;
	File AFile;
	File B1_1File;
	File B1_2File;
	File C1_1File;
	File B11_1File;

	@Before
	public void setUp() {
		Directory.root = new Directory();
		A = new Directory("A", Directory.root);
		B = new Directory("B", Directory.root);
		C = new Directory("C", Directory.root);
		A1 = new Directory("A1", A);
		B1 = new Directory("B1", B);
		C1 = new Directory("C1", C);
		B11 = new Directory("B11", B1);
		C11 = new Directory("C11", C1);
		AFile = new File("AFile", A, "AFile Contents");
		B1_1File = new File("B1_1File", B1);
		B1_2File = new File("B1_2File", B1, "B1_2File Contents");
		C1_1File = new File("C1_1File", C1);
		B11_1File = new File("B11_1File", B11);
		Directory.root.setWorkingDirectory();
	}

	@Test
	public void testMoveFileToFile() {
		FileMovement.mv(AFile, B1_2File, "");
		assertFalse(A.getFiles().contains(AFile));
		assertTrue(B1.getFiles().contains(AFile));
		assertFalse(B1.getFiles().contains(B1_2File));
		assertEquals(AFile.getContents(), "AFile Contents");
		// check to ensure B1 still contains old files and directories
		assertTrue(B1.getFiles().contains(B1_1File));
		assertTrue(B1.getChildren().contains(B11));
	}

	@Test
	public void testMoveFileToFileNoTarget() {
		Input.enteredCommand("mv /A/AFile /C/C1/C11/C11File");
		// FileMovement.mv(AFile, C11, "");
		assertFalse(A.getFiles().contains(AFile));
		assertTrue(C11.getFiles().contains(AFile));
		assertEquals(AFile.getContents(), "AFile Contents");
		assertEquals(AFile.getName(), "C11File");
		// check to ensure B1 still contains old files and directories
	}

	@Test
	public void testMoveFileToDirectory() {
		FileMovement.mv(AFile, C1, "");
		assertFalse(A.getFiles().contains(AFile));
		assertTrue(C1.getFiles().contains(AFile));
		assertEquals(AFile.getContents(), "AFile Contents");
		// check to ensure C1 still contains old files and directories
		assertTrue(C1.getFiles().contains(C1_1File));
		assertTrue(C1.getChildren().contains(C11));
	}

	@Test
	public void testMoveDirectoryToDirectory() {
		FileMovement.mv(A, C1, "");
		assertFalse(Directory.root.getChildren().contains(A));
		assertTrue(C1.getChildren().contains(A));
		// check to make sure A still contains its files and children
		assertTrue(A.getChildren().contains(A1));
		assertTrue(A.getFiles().contains(AFile));
	}

	@Test
	public void testCopyFileToFile() {
		FileMovement.cp(AFile, B1_2File, "");
		File oldFile = (File) A.fileSystemNameToObject("AFile");
		File newFile = (File) B1.fileSystemNameToObject("AFile");
		// make sure AFile still exists in dir A
		assertTrue(A.getFiles().contains(oldFile));
		// check that file was copied
		assertTrue(B1.getFiles().contains(newFile));
		// check that old file was replaced
		assertFalse(B1.getFiles().contains(B1_2File));
		assertEquals(oldFile.getContents(), "AFile Contents");
		assertEquals(newFile.getContents(), "AFile Contents");
		// check to ensure B1 still contains old files and directories
		assertTrue(B1.getFiles().contains(B1_1File));
		assertTrue(B1.getChildren().contains(B11));
		// Change contents of old AFile and make sure they don't
		// change in new AFile
		oldFile.alterContents("Try new contents", true);
		assertEquals(oldFile.getContents(), "Try new contents");
		assertEquals(newFile.getContents(), "AFile Contents");
	}

	@Test
	public void testCopyFileToDirectory() {
		FileMovement.cp(AFile, B1, "");
		File oldFile = (File) A.fileSystemNameToObject("AFile");
		File newFile = (File) B1.fileSystemNameToObject("AFile");
		// make sure AFile still exists in dir A
		assertTrue(A.getFiles().contains(oldFile));
		// check that file was copied
		assertTrue(B1.getFiles().contains(newFile));
		assertEquals(oldFile.getContents(), "AFile Contents");
		assertEquals(newFile.getContents(), "AFile Contents");
		// check to ensure B1 still contains old files and directories
		assertTrue(B1.getFiles().contains(B1_1File));
		assertTrue(B1.getChildren().contains(B11));
		// Change contents of old AFile and make sure they don't
		// change in new AFile
		oldFile.alterContents("Try new contents", true);
		assertEquals(oldFile.getContents(), "Try new contents");
		assertEquals(newFile.getContents(), "AFile Contents");
	}

	@Test
	public void testCopyDirectoryToDirectory() {
		FileMovement.cp(A, B1, "");
		Directory oldDir = (Directory) Directory.root
				.fileSystemNameToObject("A");
		Directory newDir = (Directory) B1.fileSystemNameToObject("A");
		// make sure A still exists in dir root
		assertTrue(Directory.root.getChildren().contains(oldDir));
		// check that the dir was copied
		assertTrue(B1.getChildren().contains(newDir));
		// check to ensure B1 still contains old files and directories
		assertTrue(B1.getFiles().contains(B1_1File));
		assertTrue(B1.getChildren().contains(B11));
		// Make sure the new directory contains all its children and files
		assertTrue(newDir.fileSystemInDirectory("A1"));
		assertTrue(newDir.fileSystemInDirectory("AFile"));
		// Make sure the old directory still contains all its children and files
		assertTrue(oldDir.getChildren().contains(A1));
		assertTrue(oldDir.getFiles().contains(AFile));
		// Delete old A directory and make sure new A directory still exists
		oldDir.deleteFileSystem();
		assertFalse(Directory.root.fileSystemInDirectory("A"));
		assertTrue(B1.fileSystemInDirectory("A"));
	}

}
