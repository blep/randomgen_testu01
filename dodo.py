#! /usr/bin/doit -f

from pathlib import Path

DOIT_CONFIG = {'default_tasks': ['small_crush']}

BIG_CRUSH_RNG = """pcg32_random_r pcglite32_random murmur1_counter murmur2_counter murmur3_counter siphash24_counter
	siphash24_key_counter siphash14_key_counter siphash12_key_counter
	siphash24_key_counter_64
	""".split()

CRUSH_RNG = BIG_CRUSH_RNG + """
	siphash11_key_counter
 	mt19937 mt19937_64
 	minstd_rand minstd_rand0 ranlux24 ranlux48 knuth_b
 	random_device
""".split()

SMALL_CRUSH_RNG = ["raw_counter"] + CRUSH_RNG

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
	yield from do_crushes(SMALL_CRUSH_RNG, 'small_crush')

def task_crush():
	"""Runs crush tests (~30mn per rng)."""
	yield from do_crushes(CRUSH_RNG, 'crush')

def task_big_crush():
	"""Runs crush tests (~4h per rng)."""
	yield from do_crushes(BIG_CRUSH_RNG, 'big_crush')

