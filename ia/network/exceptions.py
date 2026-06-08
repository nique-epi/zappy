"""Custom exceptions for the Zappy network layer."""


class NetworkError(Exception):
    """Base class for all network-layer errors raised by the client."""


class PlayerDeadError(NetworkError):
    """Raised when the server reports the player is dead (``dead``)."""
