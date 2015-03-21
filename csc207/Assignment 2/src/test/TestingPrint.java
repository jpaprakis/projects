package test;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import driver.Directory;
import driver.File;
import driver.Print;

public class TestingPrint {

	Directory root;
	Directory A;
	Directory B;
	Directory C;
	Directory A1;
	File aFile;

	@Before
	public void SetUp() {
		root = new Directory();
		A = new Directory("A", root);
		B = new Directory("B", A);
		C = new Directory("C", B);
		A1 = new Directory("A1", C);

	}

	@Test
	public void TestprintDirContents() {

		A.setWorkingDirectory();
		assertEquals(Print.printDirContents("A"), "A: B \n");
	}

	@Test
	public void Testman() {
		assertEquals(Print.man("hello"), "No Manual entry hello");
		assertEquals(
				Print.man("cp"),
				"cp OLDPATH  NEWPATH\n"
						+ " DESCRIPTION: Copies file or "
						+ "directories in OLDPATH to NEWPATH,");
	}

	@Test
	public void TestprintEcho() {
		assertEquals(Print.printEcho("hello"), "hello");

	}

	@Test
	public void TestprintWorkingDirectory() {
		A.setWorkingDirectory();
		assertEquals(Print.printWorkingDirectory(), "/A");
		B.setWorkingDirectory();
		assertEquals(Print.printWorkingDirectory(), "/A/B");
		C.setWorkingDirectory();
		assertEquals(Print.printWorkingDirectory(), "/A/B/C");
		A1.setWorkingDirectory();
		assertEquals(Print.printWorkingDirectory(), "/A/B/C/A1");

	}

}
