#! /usr/bin/doit -f

from pathlib import Path

DOIT_CONFIG = {'default_tasks': ['small_crush']}

COMMON_RNG = """pcg32_random_r pcglite32_random murmur1_counter murmur2_counter murmur3_counter siphash24_counter
	siphash24_key_counter siphash14_key_counter siphash12_key_counter siphash11_key_counter
	siphash24_key_counter_64""".split()
SMALL_RNG = ["raw_counter"] + COMMON_RNG

def do_crushes(rngs, kind):
	for name in rngs:
		target = Path('results/%s-%s.txt' % (kind, name))
		if not target.exists():
			yield {
				'name': name,
				'actions': ['build/bin/bbattery_runner %s %s > %s' % (name, kind, target)],
				'targets': [str(target)],
				'file_dep': []
			}



def task_small_crush():
	"""Runs small_crush tests (a few minutes)."""	
	yield from do_crushes(SMALL_RNG, 'small_crush')

def task_crush():
	"""Runs crush tests (~30mn per rng)."""
	yield from do_crushes(COMMON_RNG, 'crush')

def task_big_crush():
	"""Runs crush tests (~4h per rng)."""
	yield from do_crushes(COMMON_RNG, 'big_crush')

