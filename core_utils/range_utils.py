import re

_range_re = re.compile(r'^\d+(-\d+)?(?:,\d+(-\d+)?)*$')

def parse_ranges(text: str) -> set[int]:
    text = text.strip()
    if not text:
        return set()
    if not _range_re.match(text):
        raise ValueError(f"Invalid page‐range format: '{text}'")
    pages = set()
    for part in text.split(','):
        if '-' in part:
            start, end = part.split('-')
            start, end = int(start), int(end)
            if end < start:
                raise ValueError(f"Range end {end} is before start {start}")
            pages.update(range(start, end+1))
        else:
            pages.add(int(part))
    return pages

def format_ranges(pages: set[int]) -> str:
    if not pages:
        return ""
    pages = sorted(pages)
    ranges = []
    start = prev = pages[0]
    for p in pages[1:]:
        if p == prev + 1:
            prev = p
        else:
            if start == prev:
                ranges.append(f"{start}")
            else:
                ranges.append(f"{start}-{prev}")
            start = prev = p
    # last chunk
    if start == prev:
        ranges.append(f"{start}")
    else:
        ranges.append(f"{start}-{prev}")
    return ",".join(ranges)