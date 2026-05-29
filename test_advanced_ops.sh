#!/bin/bash

echo "=========================================="
echo "Tests for Advanced Network Operations"
echo "=========================================="

BINARY="./ipcalc"

# Test 1: Check IP (positive)
echo -e "\n[Test 1] Check IP - Positive"
$BINARY 192.168.0.0/24 --check-ip 192.168.0.50 | grep -A1 "Verificação"

# Test 2: Check IP (negative)
echo -e "\n[Test 2] Check IP - Negative"
$BINARY 192.168.0.0/24 --check-ip 192.168.1.50 | grep -A1 "Verificação"

# Test 3: Compare networks (no overlap)
echo -e "\n[Test 3] Compare Networks - No Overlap"
$BINARY 192.168.0.0/24 --compare-nets 192.168.1.0/24 | grep "Status"

# Test 4: Compare networks (overlap)
echo -e "\n[Test 4] Compare Networks - With Overlap"
$BINARY 192.168.0.0/23 --compare-nets 192.168.1.0/24 | grep -A1 "Status"

# Test 5: List range
echo -e "\n[Test 5] List Range"
$BINARY 192.168.0.0/28 --list-range | head -8

# Test 6: Merge 2 subnets
echo -e "\n[Test 6] Merge 2 Subnets"
$BINARY 192.168.0.0/25 --merge 192.168.0.128/25 | grep -A2 "Supernet"

# Test 7: Merge 3 subnets
echo -e "\n[Test 7] Merge 3 Subnets"
$BINARY 10.0.0.0/26 --merge 10.0.0.64/26 10.0.0.128/26 | grep -A2 "Supernet"

echo -e "\n=========================================="
echo "All tests completed!"
echo "=========================================="
