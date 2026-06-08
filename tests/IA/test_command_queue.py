"""
Unit tests for the CommandQueue class.
"""
from ia.network.command_queue import CommandQueue, MAX_PENDING


def test_empty_queue():
    """
    Given a fresh command queue
    When inspecting it
    Then nothing is pending, it is empty and pop_sendable returns None
    """
    queue = CommandQueue()
    assert queue.pending == 0
    assert len(queue) == 0
    assert queue.pop_sendable() is None


def test_enqueue_and_pop_increments_pending():
    """
    Given a queue with one enqueued command
    When it is popped for sending
    Then the command is returned and the pending counter increases
    """
    queue = CommandQueue()
    queue.enqueue("Forward")
    assert len(queue) == 1
    assert queue.pop_sendable() == "Forward"
    assert queue.pending == 1
    assert len(queue) == 0
    assert queue.pop_sendable() is None


def test_pending_limit_blocks_sending():
    """
    Given more commands enqueued than the pending limit
    When draining all sendable commands
    Then only MAX_PENDING are released and the rest stay queued
    """
    queue = CommandQueue()
    for i in range(MAX_PENDING + 2):
        queue.enqueue(f"cmd{i}")

    sent = []
    while (command := queue.pop_sendable()) is not None:
        sent.append(command)

    assert len(sent) == MAX_PENDING
    assert queue.pending == MAX_PENDING
    assert len(queue) == 2


def test_on_response_frees_slot():
    """
    Given a queue saturated at the pending limit with one command waiting
    When a response is acknowledged
    Then a slot frees and the waiting command becomes sendable
    """
    queue = CommandQueue()
    for i in range(MAX_PENDING + 1):
        queue.enqueue(f"cmd{i}")
    while queue.pop_sendable() is not None:
        pass
    assert queue.pending == MAX_PENDING
    assert len(queue) == 1

    queue.on_response()
    assert queue.pending == MAX_PENDING - 1
    assert queue.pop_sendable() == f"cmd{MAX_PENDING}"
    assert queue.pending == MAX_PENDING
    assert len(queue) == 0


def test_on_response_floor_at_zero():
    """
    Given a queue with no pending commands
    When a response is acknowledged
    Then the pending counter stays at zero
    """
    queue = CommandQueue()
    queue.on_response()
    assert queue.pending == 0
