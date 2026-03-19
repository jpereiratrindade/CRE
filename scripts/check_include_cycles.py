#!/usr/bin/env python3

import argparse
import pathlib
import re
import sys


INCLUDE_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')
SOURCE_EXTS = {".hpp", ".h", ".cpp", ".cc", ".cxx"}


def gather_files(roots):
    files = []
    for root in roots:
        if not root.exists():
            continue
        for path in root.rglob("*"):
            if path.is_file() and path.suffix in SOURCE_EXTS:
                files.append(path)
    return files


def build_graph(files, roots):
    root_set = [root.resolve() for root in roots]
    graph = {}
    file_set = {path.resolve() for path in files}

    for path in files:
        resolved = path.resolve()
        graph.setdefault(resolved, set())
        for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
            match = INCLUDE_RE.match(line)
            if not match:
                continue
            include_name = match.group(1)
            candidates = [path.parent / include_name]
            candidates.extend(root / include_name for root in root_set)
            for candidate in candidates:
                candidate = candidate.resolve()
                if candidate in file_set:
                    graph[resolved].add(candidate)
                    break
    return graph


def detect_cycle(graph):
    visited = {}
    stack = []

    def dfs(node):
        visited[node] = "visiting"
        stack.append(node)
        for neighbor in graph.get(node, ()):
            state = visited.get(neighbor)
            if state == "visiting":
                cycle_start = stack.index(neighbor)
                return stack[cycle_start:] + [neighbor]
            if state != "visited":
                cycle = dfs(neighbor)
                if cycle:
                    return cycle
        stack.pop()
        visited[node] = "visited"
        return None

    for node in graph:
        if visited.get(node) == "visited":
            continue
        cycle = dfs(node)
        if cycle:
            return cycle
    return None


def main():
    parser = argparse.ArgumentParser(description="Fail on include-cycle detection across project sources.")
    parser.add_argument("--roots", nargs="+", required=True, help="Roots to scan for local include dependency cycle checks")
    args = parser.parse_args()

    roots = [pathlib.Path(root) for root in args.roots]
    files = gather_files(roots)
    graph = build_graph(files, roots)
    cycle = detect_cycle(graph)
    if cycle:
        print("[include-cycle] FAIL: include dependency cycle detected:")
        for path in cycle:
            print(f" - {path}")
        return 1

    print(f"[include-cycle] OK: scanned {len(files)} file(s), no include dependency cycle found.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
