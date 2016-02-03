// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

namespace Microsoft.Azure.Devices.Client.Common
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using Microsoft.Azure.Devices.Client.Extensions;

    public class UrlEncodedDictionarySerializer
    {
        public const char KeyValueSeparator = '=';
        public const char PropertySeparator = '&';
        public const char NotAllowedCharacter = '/';
        public const int PropertySeparatorLength = 1;

        //We assume that in avarage 20% of string are the encoded characters.
        //We can make a better estimation though.
        const float EncodedSymbolsFactor = 1.2f;

        readonly IDictionary<string, string> output;
        readonly Tokenizer tokenizer;

        UrlEncodedDictionarySerializer(
            IDictionary<string, string> output,
            string value,
            int startIndex)
        {
            this.output = output;
            this.tokenizer = new Tokenizer(value, startIndex);
        }

        void Deserialize()
        {
            string key = null;

            foreach (Token token in this.tokenizer.GetTokens())
            {
                if (token.Type == TokenType.Key)
                {
                    key = token.Value;
                }
                if (token.Type == TokenType.Value)
                {
                    this.output[key] = token.Value;
                }
            }
        }

        public static Dictionary<string, string> Deserialize(string value, int startIndex)
        {
            var properties = new Dictionary<string, string>();
            Deserialize(value, startIndex, properties);
            return properties;
        }

        public static void Deserialize(string value, int startIndex, IDictionary<string, string> properties)
        {
            if (value.Length < startIndex)
            {
                return;
            }

            var parser = new UrlEncodedDictionarySerializer(properties, value, startIndex);
            parser.Deserialize();
        }

        public static string Serialize(IEnumerable<KeyValuePair<string, string>> properties)
        {
            IList<KeyValuePair<string, string>> keyValuePairs = properties as IList<KeyValuePair<string, string>> ?? properties.ToList();
            if (properties == null || !keyValuePairs.Any())
            {
                return string.Empty;
            }

            KeyValuePair<string, string>? firstProperty = null;
            int propertiesCount = 0;
            int estimatedLength = 0;
            foreach (KeyValuePair<string, string> property in keyValuePairs)
            {
                if (propertiesCount == 0)
                {
                    firstProperty = property;
                }
                
                //In case of value, '=' and ',' take up length, otherwise just ','
                estimatedLength += property.Key.Length + (property.Value?.Length + 2 ?? 1);
                propertiesCount++;
            }

            //Optimization for most common case: only correlation ID is present
            if (propertiesCount == 1 && firstProperty.HasValue)
            {
                return firstProperty.Value.Value == null ?
                    Uri.EscapeDataString(firstProperty.Value.Key) :
                    Uri.EscapeDataString(firstProperty.Value.Key) + KeyValueSeparator + Uri.EscapeDataString(firstProperty.Value.Value);
            }

            var propertiesBuilder = new StringBuilder((int)(estimatedLength * EncodedSymbolsFactor));

            foreach (KeyValuePair<string, string> property in keyValuePairs)
            {
                propertiesBuilder.Append(Uri.EscapeDataString(property.Key));
                if (property.Value != null)
                {
                    propertiesBuilder
                        .Append(KeyValueSeparator)
                        .Append(Uri.EscapeDataString(property.Value));
                }
                propertiesBuilder.Append(PropertySeparator);
            }
            return propertiesBuilder.Length == 0 ? string.Empty : propertiesBuilder.ToString(0, propertiesBuilder.Length - PropertySeparatorLength);
        }

        public enum TokenType
        {
            Key,
            Value
        }

        struct Token
        {
            public TokenType Type { get; }

            public string Value { get; }

            public Token(TokenType tokenType, string value)
            {
                this.Type = tokenType;
                this.Value = value == null ? null : Uri.UnescapeDataString(value);
            }
        }

        /// <summary>
        /// Tokenizer state machine
        /// </summary>
        class Tokenizer
        {
            enum TokenizerState
            {
                ReadyToReadKey,
                ReadKey,
                ReadValue,
                Error,
                Finish
            }
            readonly string value;
            int position;

            TokenizerState currentState = TokenizerState.ReadyToReadKey;

            public Tokenizer(string value, int startIndex)
            {
                this.value = value;
                this.position = startIndex;
            }

            public IEnumerable<Token> GetTokens()
            {
                if (this.position >= this.value.Length)
                {
                    yield break;
                }
                int readCount = 0;
                bool readCompleted = false;
                string errorMessage = null;
                while (!readCompleted)
                {
                    switch (this.currentState)
                    {
                        case TokenizerState.ReadyToReadKey:
                            {
                                if (this.position >= this.value.Length)
                                {
                                    errorMessage = "Unexpected string end in '{0}' state.".FormatInvariant(this.currentState);
                                    this.currentState = TokenizerState.Error;
                                    break;
                                }
                                char currentChar = this.value[this.position];
                                switch (currentChar)
                                {
                                    case '=':
                                    case '&':
                                        errorMessage = "Unexpected character '{0}' in '{1}' state.".FormatInvariant(currentChar, this.currentState);
                                        this.currentState = TokenizerState.Error;
                                        break;
                                    case '/':
                                        this.currentState = TokenizerState.Finish;
                                        break;
                                    default:
                                        readCount++;
                                        this.currentState = TokenizerState.ReadKey;
                                        break;
                                }
                                break;
                            }
                        case TokenizerState.ReadKey:
                            {
                                if (this.position >= this.value.Length)
                                {
                                    yield return this.CreateToken(TokenType.Key, readCount);
                                    yield return this.CreateToken(TokenType.Value, 0);
                                    readCount = 0;
                                    this.currentState = TokenizerState.Finish;
                                    break;
                                }
                                char currentChar = this.value[this.position];
                                switch (currentChar)
                                {
                                    case '=':
                                        yield return this.CreateToken(TokenType.Key, readCount);
                                        readCount = 0;
                                        this.currentState = TokenizerState.ReadValue;
                                        break;
                                    case '&':
                                        yield return this.CreateToken(TokenType.Key, readCount);
                                        yield return this.CreateToken(TokenType.Value, 0);
                                        readCount = 0;
                                        this.currentState = TokenizerState.ReadyToReadKey;
                                        break;
                                    case '/':
                                        yield return this.CreateToken(TokenType.Key, readCount);
                                        yield return this.CreateToken(TokenType.Value, 0);
                                        readCount = 0;
                                        this.currentState = TokenizerState.Finish;
                                        break;
                                    default:
                                        readCount++;
                                        //this.currentState = TokenizerState.ReadKey;
                                        break;
                                }
                                break;
                            }
                        case TokenizerState.ReadValue:
                            {
                                if (this.position >= this.value.Length)
                                {
                                    yield return this.CreateToken(TokenType.Value, readCount);
                                    readCount = 0;
                                    this.currentState = TokenizerState.Finish;
                                    break;
                                }
                                char currentChar = this.value[this.position];
                                switch (currentChar)
                                {
                                    case '=':
                                        errorMessage = "Unexpected character '{0}' in '{1}' state.".FormatInvariant(currentChar, this.currentState);
                                        this.currentState = TokenizerState.Error;
                                        break;
                                    case '&':
                                        yield return this.CreateToken(TokenType.Value, readCount);
                                        readCount = 0;
                                        this.currentState = TokenizerState.ReadyToReadKey;
                                        break;
                                    case '/':
                                        yield return this.CreateToken(TokenType.Value, readCount);
                                        readCount = 0;
                                        this.currentState = TokenizerState.Finish;
                                        break;
                                    default:
                                        readCount++;
                                        //this.currentState = TokenizerState.ReadValue;
                                        break;
                                }
                                break;
                            }
                        case TokenizerState.Finish:
                        case TokenizerState.Error:
                            readCompleted = true;
                            break;
                        default:
                            throw new NotSupportedException();
                    }
                    this.position++;
                }

                if (this.currentState == TokenizerState.Error)
                {
                    throw new FormatException(errorMessage);
                }
            }

            Token CreateToken(TokenType tokenType, int readCount)
            {
                string tokenValue = readCount == 0 ? null : this.value.Substring(this.position - readCount, readCount);

                return new Token(tokenType, tokenValue);
            }
        }
    }
}