// Copyright Epic Games, Inc. All Rights Reserved.

using AutomationTool;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BuildScripts.Automation
{
	[RequireP4]
	[DoesNotNeedP4CL]
	[Help("Checks that the casing of files within a path on a case-insensitive Perforce server is correct.")]
	[Help("Path", "The path to query")]
	class CheckPerforceCase : BuildCommand
	{
		class TreeNode
		{
			public string Path;
			public List<string> Files = new List<string>();
			public Dictionary<string, TreeNode> ChildNodes = new Dictionary<string, TreeNode>(StringComparer.Ordinal);
			public int Count;

			public TreeNode(string Path)
			{
				this.Path = Path;
			}

			public void Add(string DepotPath, int Idx)
			{
				int NextIdx = DepotPath.IndexOf('/', Idx);
				if(NextIdx == -1)
				{
					Files.Add(DepotPath);
				}
				else
				{
					string Fragment = DepotPath.Substring(Idx, NextIdx - Idx);

					TreeNode ChildNode;
					if (!ChildNodes.TryGetValue(Fragment, out ChildNode))
					{
						ChildNode = new TreeNode(DepotPath.Substring(0, NextIdx));
						ChildNodes.Add(Fragment, ChildNode);
					}
					ChildNode.Add(DepotPath, NextIdx + 1);
				}
				Count++;
			}

			public IEnumerable<string> EnumerateFiles()
			{
				foreach (string File in Files)
				{
					yield return File;
				}
				foreach (TreeNode ChildNode in ChildNodes.Values)
				{
					foreach (string ChildFile in ChildNode.EnumerateFiles())
					{
						yield return ChildFile;
					}
				}
			}

			public int PrintIssues()
			{
				int NumIssues = 0;
				foreach (IGrouping<string, KeyValuePair<string, TreeNode>> Group in ChildNodes.GroupBy(x => x.Key, StringComparer.OrdinalIgnoreCase))
				{
					if (Group.Count() > 1)
					{
						StringBuilder Message = new StringBuilder();
						Message.AppendFormat("Inconsistent casing for {0}:", Group.First().Value.Path);
						foreach (KeyValuePair<string, TreeNode> Pair in Group)
						{
							Message.AppendFormat("\n  Could be '{0}':", Pair.Key, Pair.Value.Count, (Pair.Value.Count == 1)? "file" : "files");

							int NumFiles = 0;
							foreach (string File in Pair.Value.EnumerateFiles())
							{
								Message.AppendFormat("\n    {0}", File);
								if (++NumFiles >= 10)
								{
									break;
								}
							}

							Message.AppendFormat("\n    ({0} {1})", Pair.Value.Count, (Pair.Value.Count == 1) ? "file" : "files");
						}
						LogError(Message.ToString());
						NumIssues++;
					}

					foreach (KeyValuePair<string, TreeNode> Pair in Group)
					{
						NumIssues += Pair.Value.PrintIssues();
					}
				}
				return NumIssues;
			}
		}

		public override void ExecuteBuild()
		{
			// Parse the path
			string Filter = ParseRequiredStringParam("Filter");
			if(!Filter.StartsWith("//"))
			{
				throw new AutomationException("Filter is not a depot path");
			}

			// Find all the matching files
			LogInformation("Finding files matching {0}", Filter);
			List<string> Files = P4.Files(String.Format("-e {0}", Filter));
			LogInformation("Found {0} files", Files.Count);

			// Build a tree of all the conflicting paths and print them out
			TreeNode RootNode = new TreeNode("//");
			foreach (string File in Files)
			{
				if (!File.StartsWith("//", StringComparison.Ordinal))
				{
					LogWarning("File '{0}' does not start with '//'", File);
					continue;
				}
				RootNode.Add(File, 2);
			}
			RootNode.PrintIssues();
		}
	}
}
