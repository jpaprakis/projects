package test;

import static org.junit.Assert.*;

import org.junit.*;

import driver.*;

public class TestingStackCommands {

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
	public void TestPushd() {
		// pushing current Directory on to the stack
		Directory.root.setWorkingDirectory();
		StackCommands.Pushd(A);
		assertEquals(Directory.workingDirectory, A);
		assertEquals(StackCommands.getTopOfStack(), Directory.root);
		// pushing another Directory
		StackCommands.Pushd(B);
		assertEquals(Directory.workingDirectory, B);
		assertEquals(StackCommands.getTopOfStack(), A);
	}
	
	@Test
	public void TestPopd() {
		//popping the directory
		Directory.root.setWorkingDirectory();
		StackCommands.Pushd(A);
		StackCommands.Pushd(B);
		StackCommands.Popd();
		assertEquals(Directory.workingDirectory, A);
		assertEquals(StackCommands.getTopOfStack(), Directory.root);
	}
}
