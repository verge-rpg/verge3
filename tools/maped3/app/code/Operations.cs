using System;
using System.Collections;

//TODO
//FUCK
//this whole system needs for undo() and exec() to take an argument called
//object document
//so they know which document theyre supposed to act on
//FUCK
	
namespace winmaped2
{

	namespace Operations
	{

		public class OperationManager
		{
			Stack groups = new Stack();
			Stack operations = new Stack();
			Stack undos = new Stack();

			public void undo()
			{
				if(currOperation == null)
					return;
				else
				{
					currOperation.undo();
					undos.Push(operations.Pop());
				}
			}

			public void redo()
			{
				if(undos.Count == 0)
					return;
				IOperation op = (IOperation)undos.Pop();
				operations.Push(op);
				op.exec();
			}

			public void add(IOperation op)
			{
				if(currGroup == null)
					operations.Push(op);
				else
					currGroup.add(op);
				
				undos.Clear();
			}

			public void addExec(IOperation op)
			{
				add(op);
				op.exec();
			}

			public bool isEmpty { get { return operations.Count==0; } }
			public bool canRedo { get { return undos.Count != 0; } }
			
			public IOperation currRedo { get { return (IOperation)undos.Peek(); } }
			public string currRedoName { get { return currRedo.name; } }

			public IOperation currOperation { get { return operations.Count==0?null:(IOperation)operations.Peek(); } }
			public string currOperationName { get { return currOperation==null?"":currOperation.name; } }
			public OperationGroup currGroup { get { return (OperationGroup)groups.Peek(); } }
			public void beginGroup(string name)
			{
				OperationGroup opGroup = new OperationGroup();
				opGroup.name = name;
				if(groups.Count == 0)
				{
					operations.Push(opGroup);
					groups.Push(opGroup);
				}
				else
					currGroup.add(opGroup);
			}

			public void endGroup()
			{
				if(groups.Count == 0)
					throw new Exception("wtf, you ended a group when you werent currently in one");
				groups.Pop();
			}

			public void endGroupExec()
			{
				if(groups.Count == 0)
					throw new Exception("wtf, you ended a group when you werent currently in one");
				((OperationGroup)groups.Pop()).exec();
			}
		}

		public class OperationBase
		{
			string m_name = "";
			public string name { get { return m_name; } set { m_name = value; } }
		}

		public class OperationGroup : OperationBase, IOperation
		{
			public void exec()
			{
				foreach(IOperation op in childOps)
					op.exec();
			}
			public void undo()
			{
				for(int i=0;i<childOps.Count;i++)
					((IOperation)childOps[childOps.Count-1-i]).undo();
			}

			ArrayList childOps = new ArrayList();
		
			public void add(IOperation op)
			{
				childOps.Add(op);
			}

		}

		public interface IOperation
		{
			void exec();
			void undo();
			string name { get; }
		}
	}
}